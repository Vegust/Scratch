//!shader vertex
#version 460 core

layout (location = 0) in vec4 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoords;

out GS_OUT {
	vec3 g_Normal;
	vec3 g_FragPos;
	vec2 g_TexCoords;
	vec4 g_FragPosLightSpace;
} vs_out;

uniform mat4 u_MVP;
uniform mat4 u_Model;
uniform mat4 u_ViewModel;
uniform mat3 u_NormalMatrix;
uniform mat4 u_LightProjectionView;
uniform bool u_Shadowmaps;

void main() {
	vs_out.g_Normal = u_NormalMatrix * Normal;
	vs_out.g_FragPos = vec3(u_ViewModel * Position);
	vs_out.g_TexCoords = TexCoords;
	vs_out.g_FragPosLightSpace = u_LightProjectionView * u_Model * Position;
	gl_Position = u_MVP * Position;
};

////!shader geometry
//#version 460 core
//layout (triangles) in;
//layout (triangle_strip, max_vertices = 3) out;
//
//in VS_OUT {
//	vec3 v_Normal;
//	vec3 v_FragPos;
//	vec2 v_TexCoords;
//} gs_in[];
//
//out GS_OUT {
//	vec3 g_Normal;
//	vec3 g_FragPos;
//	vec2 g_TexCoords;
//} gs_out;
//
//void main() {
//	for (int i = 0; i < 3; ++i)
//	{
//		gl_Position = gl_in[i].gl_Position;
//		gs_out.g_Normal = gs_in[i].v_Normal;
//		gs_out.g_FragPos = gs_in[i].v_FragPos;
//		gs_out.g_TexCoords = gs_in[i].v_TexCoords;
//		EmitVertex();
//	}
//	EndPrimitive();
//}

//!shader fragment
#version 460 core

struct material {
	sampler2D DiffuseMap;
	sampler2D SpecularMap;
	sampler2D EmissionMap;
	float Shininess;
};

struct light {
	int Type; // light_type: 0 - point, 1 - directional, 2 - spot

	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;

	vec3 Position;
	vec3 Direction;

	float AttenuationRadius;
	float AngularAttenuation;
	float AngularAttenuationFalloffStart;
};

in GS_OUT {
	vec3 g_Normal;
	vec3 g_FragPos;
	vec2 g_TexCoords;
	vec4 g_FragPosLightSpace;
} vs_in;

out vec4 Color;

uniform material u_Material;

#define MAX_LIGHTS 100
uniform light u_Lights[MAX_LIGHTS];
uniform int u_NumLights;

uniform mat4 u_LightProjectionView;

uniform bool u_Shadowmaps;
uniform sampler2D u_Shadowmap;

uniform bool u_Unlit;
uniform bool u_Depth;

float ShadowCalculation(vec4 FragPosLightSpace, vec3 Normal, vec3 LightDirection)
{
	vec3 ProjCoords = vs_in.g_FragPosLightSpace.xyz / vs_in.g_FragPosLightSpace.w;
	if (ProjCoords.z > 1.0)
	{
		return 0.0;
	}

	ProjCoords = ProjCoords * 0.5 + 0.5;
	float CurrentDepth = ProjCoords.z;
	float Bias = max(0.003 * (1.0 - dot(Normal, LightDirection)), 0.0006);

	// PCF
	vec2 TexelSize = 1.0 / textureSize(u_Shadowmap, 0);
	float Shadow = 0;
	for (int x = -2; x <= 2; ++x)
	{
		for (int y = -2; y <= 2; ++y)
		{
			float ClosestDepth = texture(u_Shadowmap, ProjCoords.xy + vec2(x, y) * TexelSize).r;
			Shadow += CurrentDepth - Bias > ClosestDepth ? 1.0 : 0.0;
		}
	}

	return Shadow /= 25;
}

vec3 CalcLightColor(light Light, vec3 DiffuseTextureColor, vec3 SpecularTextureColor, vec3 Normal, vec3 ViewDirection, bool bUseShadowmap)
{
	// Ambient
	vec3 AmbientColor = Light.Ambient * DiffuseTextureColor;

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
	vec3 DiffuseColor = DiffuseImpact * Light.Diffuse * DiffuseTextureColor;

	// Specular
	// Phong
	//vec3 ReflectDirection = reflect(-LightDirection, Normal);
	//float SpecularImpact = pow(max(dot(ViewDirection, ReflectDirection), 0.0), u_Material.Shininess);
	// Blinn-Phong
	vec3 HalfwayDirection = normalize(LightDirection + ViewDirection);
	float SpecularImpact = pow(max(dot(HalfwayDirection, Normal), 0.0), u_Material.Shininess);

	vec3 SpecularColor = Light.Specular * SpecularImpact * SpecularTextureColor;

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
	if (bUseShadowmap)
	{
		ShadowMult = 1.f - ShadowCalculation(vs_in.g_FragPosLightSpace, Normal, LightDirection);
	}

	return Falloff * Attenuation * (AmbientColor + ShadowMult * (DiffuseColor + SpecularColor));
}

void main() {
	vec3 CombinedLightColor = vec3(0.f, 0.f, 0.f);

	vec3 DiffuseTextureColor = vec3(texture(u_Material.DiffuseMap, vs_in.g_TexCoords));
	vec3 SpecularTextureColor = vec3(texture(u_Material.SpecularMap, vs_in.g_TexCoords));
	vec3 NormalizedNormal = normalize(vs_in.g_Normal);
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
					i == 0 && u_Shadowmaps
				);
			}

			Color = vec4(CombinedLightColor + vec3(texture(u_Material.EmissionMap, vs_in.g_TexCoords)), 1.0);
		}
	}
};
