//!shader shared
#version 460 core

#define DIFFUSE_TEXTURE_SLOT 0
#define SPECULAR_TEXTURE_SLOT 1
#define EMISSION_TEXTURE_SLOT 2
#define NORMAL_TEXTURE_SLOT 3

#define DIR_SHADOWMAP_BINDING 9
#define OMNI_SHADOWMAP_BINDING 10

struct material {
	float Shininess;
};

struct light {
	vec3 Color;
	float AmbientStrength;
	vec3 Position;
	float AttenuationRadius;
	vec3 Direction;
	float AngularAttenuation;
	mat4 ShadowMatrix;
	float AngularAttenuationFalloffStart;
	int Type; // light_type: 0 - point, 1 - directional, 2 - spot
};

// global
layout (std140, binding = 0) uniform u_Global {
	mat4 u_Projection;
	bool u_Unlit;
	bool u_Depth;
};

// view
uniform mat4 u_View;
uniform mat4 u_InvertedView;

// material
uniform material u_Material;
layout (binding = 0) uniform sampler2D u_Textures[8];

// lights
uniform int u_NumLights;
layout (std140, binding = 0) restrict readonly buffer ssbo_Lights {
	light u_Lights[];
};
layout (binding = DIR_SHADOWMAP_BINDING) uniform sampler2D u_Shadowmap;
layout (binding = OMNI_SHADOWMAP_BINDING) uniform samplerCube u_PointShadowmap;

// model
uniform mat4 u_Model;
uniform mat4 u_ModelNormal;

struct VertexOutput {
	vec3 g_Normal;
	vec3 g_FragPos;
	vec2 g_TexCoords;
	mat3 TBN;
};

//!shader vertex
layout (location = 0) in vec4 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoords;
layout (location = 3) in vec3 Tangent;

layout (location = 0) out VertexOutput Output;

void main() {
	mat4 ViewModel = u_View * u_Model;
	mat4 ViewModelNormal = u_View * u_ModelNormal;
	Output.g_Normal = vec3(ViewModelNormal * vec4(Normal, 0.0));
	Output.g_FragPos = vec3(ViewModel * Position);
	Output.g_TexCoords = TexCoords;
	gl_Position = u_Projection * vec4(Output.g_FragPos, 1);
	vec3 T = normalize(vec3(ViewModel * vec4(Tangent, 0.0)));
	vec3 N = normalize(vec3(ViewModel * vec4(Normal, 0.0)));
	vec3 B = cross(T, N);
	Output.TBN = mat3(T, B, N);
};

//!shader fragment
layout (location = 0) in VertexOutput Input;
layout (location = 0) out vec4 Color;

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
	
	vec3 LightViewPosition = vec3(u_View * vec4(Light.Position, 1));
	vec3 LightViewDirection = vec3(u_View * vec4(Light.Direction, 0));

	// Diffuse
	vec3 LightDirection;
	if (Light.Type == 1) //directional
	{
		LightDirection = normalize(-LightViewDirection);
	}
	else
	{
		LightDirection = normalize(LightViewPosition - Input.g_FragPos);
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
		float Distance = length(LightViewPosition - Input.g_FragPos);
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
		float Theta = dot(LightDirection, normalize(-LightViewDirection));

		Falloff = clamp(1.0 - (FalloffStart - Theta) / (FalloffStart - Cutoff), 0.0, 1.0);
		if (Theta < Cutoff)
		{
			Falloff = 0.f;
		}
	}

	float ShadowMult = 1.f;
	if (ShadowmapType == 1)
	{
		ShadowMult = 1.f - ShadowCalculation(Input.g_FragPos, Normal, LightDirection, Light.ShadowMatrix);
	}
	else if (ShadowmapType == 2)
	{
		ShadowMult = 1.f - PointShadowCalculation(Input.g_FragPos, LightViewPosition, Light.AttenuationRadius);
	}

	return Falloff * Attenuation * (AmbientColor + ShadowMult * (DiffuseColor + SpecularColor));
}

void main() {
	vec3 CombinedLightColor = vec3(0.f, 0.f, 0.f);
	vec3 DiffuseTextureColor = vec3(texture(u_Textures[DIFFUSE_TEXTURE_SLOT], Input.g_TexCoords));
	vec3 SpecularTextureColor = vec3(texture(u_Textures[SPECULAR_TEXTURE_SLOT], Input.g_TexCoords));
	vec3 TexturedNormal = texture(u_Textures[NORMAL_TEXTURE_SLOT], Input.g_TexCoords).rgb;
	vec3 NormalizedNormal = length(TexturedNormal) > 0.1 ? normalize(Input.TBN * (TexturedNormal * 2.0 - 1.0)) : normalize(Input.g_Normal);
	vec3 ViewDirection = normalize(-Input.g_FragPos);

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
					i == 0 ? 1 : 2
				);
			}

			Color = vec4(CombinedLightColor + vec3(texture(u_Textures[EMISSION_TEXTURE_SLOT], Input.g_TexCoords)), 1.0);
		}
	}
}
