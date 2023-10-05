//!shader shared
#version 460 core

struct material {
	sampler2D DiffuseMap;
	sampler2D SpecularMap;
	sampler2D EmissionMap;
	sampler2D NormalMap;
	float Shininess;
};

struct light {
	int Type; // light_type: 0 - point, 1 - directional, 2 - spot
	vec3 Color;
	float AmbientStrength;
	vec3 Position;
	vec3 Direction;
	float AttenuationRadius;
	float AngularAttenuation;
	float AngularAttenuationFalloffStart;
	mat4 ShadowMatrix;
};

// global
uniform mat4 u_Projection;
uniform bool u_Unlit;
uniform bool u_Depth;

// view
uniform mat4 u_View;
uniform mat4 u_InvertedView;

// material
uniform material u_Material;

// lights
#define MAX_LIGHTS 10
uniform light u_Lights[MAX_LIGHTS];
uniform int u_NumLights;
uniform bool u_Shadowmaps;
uniform sampler2D u_Shadowmap;
uniform samplerCube u_PointShadowmap;

// model
uniform mat4 u_Model;
uniform mat4 u_ModelNormal;

//!shader vertex
layout (location = 0) in vec4 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoords;
layout (location = 3) in vec3 Tangent;

out VS_OUT {
	// all in view space
	vec3 g_Normal;
	vec3 g_FragPos;
	vec2 g_TexCoords;
	mat3 TBN;
} vs_out;

void main() {
	mat4 ViewModel = u_View * u_Model;
	mat4 ViewModelNormal = u_View * u_ModelNormal;
	vs_out.g_Normal = vec3(ViewModelNormal * vec4(Normal, 0.0));
	vs_out.g_FragPos = vec3(ViewModel * Position);
	vs_out.g_TexCoords = TexCoords;
	gl_Position = u_Projection * vec4(vs_out.g_FragPos, 1);

	vec3 T = normalize(vec3(ViewModel * vec4(Tangent, 0.0)));
	vec3 N = normalize(vec3(ViewModel * vec4(Normal, 0.0)));
	vec3 B = cross(T, N);
	vs_out.TBN = mat3(T, B, N);
};

//!shader fragment
in VS_OUT {
	vec3 g_Normal;
	vec3 g_FragPos;
	vec2 g_TexCoords;
	mat3 TBN;
} vs_in;

out vec4 Color;

float PointShadowCalculation(vec3 FragPos, vec3 PointLightPosition, float AttenuationRadius)
{
	vec3 FragToLight = FragPos - PointLightPosition;
	FragToLight = vec3(u_InvertedView * vec4(FragToLight, 0));
	float ClothestDepth = texture(u_PointShadowmap, normalize(FragToLight)).r * AttenuationRadius;
	float CurrentDepth = length(FragToLight);
	float Shadow = CurrentDepth - 0.1 > ClothestDepth ? 1.0 : 0.0;
	return Shadow;
}

float ShadowCalculation(vec3 FragPos, vec3 Normal, vec3 LightDirection, mat4 ShadowMatrix)
{
	vec4 FragPosLightSpace = ShadowMatrix * u_InvertedView * vec4(FragPos, 1);
	vec3 ProjCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
	if (ProjCoords.z > 1.0)
	{
		return 0.0;
	}

	ProjCoords = ProjCoords * 0.5 + 0.5;
	float CurrentDepth = ProjCoords.z;

	ivec2 TextureSize = textureSize(u_Shadowmap, 0);
	vec2 TexelSize = 1.0 / TextureSize;
	float LightDot = dot(Normal, LightDirection);
	float Bias = TexelSize.x / 1.5 * (4 - pow(LightDot, 2.0) * 3);
	float Shadow = 0;

	// TODO: weighted on distance to pixels so each pixel linearly interpolates between shadowmap values
	// PCF
	for (int x = -2; x <= 2; ++x)
	{
		for (int y = -2; y <= 2; ++y)
		{
			float ClosestDepth = texture(u_Shadowmap, ProjCoords.xy + vec2(x, y) * TexelSize).r;
			Shadow += CurrentDepth - Bias > ClosestDepth ? 1.0 : 0.0;
		}
	}
	Shadow = Shadow /= 25;

	return Shadow;
}

vec3 CalcLightColor(light Light, vec3 DiffuseTextureColor, vec3 SpecularTextureColor, vec3 Normal, vec3 ViewDirection, int ShadowmapType)
{
	// Ambient
	vec3 AmbientColor = Light.AmbientStrength * Light.Color * DiffuseTextureColor;

	// Diffuse
	vec3 LightDirection;
	if (Light.Type == 1) //directional
	{
		LightDirection = normalize(-Light.Direction);
	}
	else
	{
		LightDirection = normalize(Light.Position - vs_in.g_FragPos);
	}
	float DiffuseImpact = max(dot(Normal, LightDirection), 0.0);
	vec3 DiffuseColor = DiffuseImpact * Light.Color * DiffuseTextureColor;

	// Specular
	// Phong
	//vec3 ReflectDirection = reflect(-LightDirection, Normal);
	//float SpecularImpact = pow(max(dot(ViewDirection, ReflectDirection), 0.0), u_Material.Shininess);
	// Blinn-Phong
	vec3 HalfwayDirection = normalize(LightDirection + ViewDirection);
	float SpecularImpact = max(dot(Normal, LightDirection), 0.0) * pow(max(dot(HalfwayDirection, Normal), 0.0), u_Material.Shininess);

	vec3 SpecularColor = Light.Color * SpecularImpact * SpecularTextureColor;

	// Attenuation
	float Attenuation = 1.0;
	if (Light.Type == 0 /*point*/ || Light.Type == 2 /*spot*/)
	{
		// Normalized to max distance of 100. Coefficients for 100 are from
		// https://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation
		float Distance = length(Light.Position - vs_in.g_FragPos);
		//float NormalizedDistance = 100.0 * Distance / Light.AttenuationRadius;
		//Attenuation = 1.0 / (1.0 + NormalizedDistance * 0.045 + NormalizedDistance * NormalizedDistance * 0.0075);

		// When we use gamma correction we can just use correct quadratic attenuation?
		Attenuation = min(1.0, Light.AttenuationRadius / (Distance * Distance));
	}

	// Angular falloff
	float Falloff = 1.f;
	if (Light.Type == 2) //spot
	{
		float Cutoff = cos(radians(Light.AngularAttenuation));
		float FalloffStart = max(Cutoff, cos(radians(Light.AngularAttenuationFalloffStart)));
		float Theta = dot(LightDirection, normalize(-Light.Direction));

		Falloff = clamp(1.0 - (FalloffStart - Theta) / (FalloffStart - Cutoff), 0.0, 1.0);
		if (Theta < Cutoff)
		{
			Falloff = 0.f;
		}
	}

	float ShadowMult = 1.f;
	if (ShadowmapType == 1)
	{
		ShadowMult = 1.f - ShadowCalculation(vs_in.g_FragPos, Normal, LightDirection, Light.ShadowMatrix);
	}
	else if (ShadowmapType == 2)
	{
		ShadowMult = 1.f - PointShadowCalculation(vs_in.g_FragPos, Light.Position, Light.AttenuationRadius);
	}

	return Falloff * Attenuation * (AmbientColor + ShadowMult * (DiffuseColor + SpecularColor));
}

void main() {
	vec3 CombinedLightColor = vec3(0.f, 0.f, 0.f);
	vec3 DiffuseTextureColor = vec3(texture(u_Material.DiffuseMap, vs_in.g_TexCoords));
	vec3 SpecularTextureColor = vec3(texture(u_Material.SpecularMap, vs_in.g_TexCoords));
	vec3 TexturedNormal = texture(u_Material.NormalMap, vs_in.g_TexCoords).rgb;
	vec3 NormalizedNormal = length(TexturedNormal) > 0.1 ? normalize(vs_in.TBN * (TexturedNormal * 2.0 - 1.0)) : normalize(vs_in.g_Normal);
	vec3 ViewDirection = normalize(-vs_in.g_FragPos);

	if (u_Depth)
	{
		const float Near = 0.001f;
		const float Far = 100.f;
		float NDC = gl_FragCoord.z * 2.0 - 1.0;
		float LinearDepth = (2.0 * Near * Far) / (Far + Near - NDC * (Far - Near));
		float FinalValue = (LinearDepth - Near) / 100.0;
		Color = vec4(vec3(FinalValue), 1.0);
	}
	else
	{
		if (u_Unlit)
		{
			Color = vec4(DiffuseTextureColor, 1.f);
		}
		else
		{
			for (int i = 0; i < u_NumLights; ++i)
			{
				CombinedLightColor += CalcLightColor(
					u_Lights[i],
					DiffuseTextureColor,
					SpecularTextureColor,
					NormalizedNormal,
					ViewDirection,
					u_Shadowmaps ? (i == 0 ? 1 : 2) : 0
				);
			}

			Color = vec4(CombinedLightColor + vec3(texture(u_Material.EmissionMap, vs_in.g_TexCoords)), 1.0);
		}
	}
}
