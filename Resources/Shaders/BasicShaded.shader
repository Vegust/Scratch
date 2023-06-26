//!shader vertex
#version 460 core

layout (location = 0) in vec4 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoords;

out vec3 v_Normal;
out vec3 v_FragPos;
out vec2 v_TexCoords;

uniform mat4 u_MVP;
uniform mat4 u_ViewModel;
uniform mat3 u_NormalMatrix;

void main() {
	v_Normal = u_NormalMatrix * Normal;
	v_FragPos = vec3(u_ViewModel * Position);
	v_TexCoords = TexCoords;
	gl_Position = u_MVP * Position;
};

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

in vec3 v_Normal;
in vec3 v_FragPos;
in vec2 v_TexCoords;

out vec4 Color;

uniform material u_Material;
uniform light u_Light;

void main() {
	vec3 CombinedLightColor = vec3(0.f, 0.f, 0.f);

	if (u_Light.Type == 0) //point
	{
		// Ambient
		vec3 AmbientColor = u_Light.Ambient * vec3(texture(u_Material.DiffuseMap, v_TexCoords));

		// Diffuse
		vec3 NormalizedNormal = normalize(v_Normal);
		vec3 LightDirection = normalize(u_Light.Position - v_FragPos);
		float DiffuseImpact = max(dot(NormalizedNormal, LightDirection), 0.0);
		vec3 DiffuseColor = DiffuseImpact * u_Light.Diffuse * vec3(texture(u_Material.DiffuseMap, v_TexCoords));

		// Specular
		vec3 ReflectDirection = reflect(-LightDirection, v_Normal);
		vec3 ViewDirection = normalize(-v_FragPos);
		float SpecularImpact = pow(max(dot(ViewDirection, ReflectDirection), 0.0), u_Material.Shininess);
		vec3 SpecularColor = u_Light.Specular * SpecularImpact * vec3(texture(u_Material.SpecularMap, v_TexCoords));

		// Normalized to max distance of 100. Coefficients for 100 are from
		// https://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation
		float Distance = length(u_Light.Position - v_FragPos);
		float NormalizedDistance = 100.0 * Distance / u_Light.AttenuationRadius;
		float Attenuation = 1.0 / (1.0 + NormalizedDistance * 0.045 + NormalizedDistance * NormalizedDistance * 0.0075);

		CombinedLightColor += Attenuation * (AmbientColor + DiffuseColor + SpecularColor);
	}
	else if (u_Light.Type == 1) //directional
	{
		// Ambient
		vec3 AmbientColor = u_Light.Ambient * vec3(texture(u_Material.DiffuseMap, v_TexCoords));

		// Diffuse
		vec3 NormalizedNormal = normalize(v_Normal);
		vec3 LightDirection = normalize(-u_Light.Direction);
		float DiffuseImpact = max(dot(NormalizedNormal, LightDirection), 0.0);
		vec3 DiffuseColor = DiffuseImpact * u_Light.Diffuse * vec3(texture(u_Material.DiffuseMap, v_TexCoords));

		// Specular
		vec3 ReflectDirection = reflect(-LightDirection, v_Normal);
		vec3 ViewDirection = normalize(-v_FragPos);
		float SpecularImpact = pow(max(dot(ViewDirection, ReflectDirection), 0.0), u_Material.Shininess);
		vec3 SpecularColor = u_Light.Specular * SpecularImpact * vec3(texture(u_Material.SpecularMap, v_TexCoords));

		CombinedLightColor += AmbientColor + DiffuseColor + SpecularColor;
	}
	else if (u_Light.Type == 2) //spot
	{
		// Ambient
		vec3 AmbientColor = u_Light.Ambient * vec3(texture(u_Material.DiffuseMap, v_TexCoords));

		// Diffuse
		vec3 NormalizedNormal = normalize(v_Normal);
		vec3 LightDirection = normalize(u_Light.Position - v_FragPos);
		float DiffuseImpact = max(dot(NormalizedNormal, LightDirection), 0.0);
		vec3 DiffuseColor = DiffuseImpact * u_Light.Diffuse * vec3(texture(u_Material.DiffuseMap, v_TexCoords));

		// Specular
		vec3 ReflectDirection = reflect(-LightDirection, v_Normal);
		vec3 ViewDirection = normalize(-v_FragPos);
		float SpecularImpact = pow(max(dot(ViewDirection, ReflectDirection), 0.0), u_Material.Shininess);
		vec3 SpecularColor = u_Light.Specular * SpecularImpact * vec3(texture(u_Material.SpecularMap, v_TexCoords));

		// Normalized to max distance of 100. Coefficients for 100 are from
		// https://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation
		float Distance = length(u_Light.Position - v_FragPos);
		float NormalizedDistance = 100.0 * Distance / u_Light.AttenuationRadius;
		float Attenuation = 1.0 / (1.0 + NormalizedDistance * 0.045 + NormalizedDistance * NormalizedDistance * 0.0075);

		float Cutoff = cos(radians(u_Light.AngularAttenuation));
		float FalloffStart = max(Cutoff, cos(radians(u_Light.AngularAttenuationFalloffStart)));
		float Theta = dot(LightDirection, normalize(-u_Light.Direction));

		float Falloff = clamp(1.0 - (FalloffStart - Theta) / (FalloffStart - Cutoff), 0.0, 1.0);

		if (Theta > Cutoff)
		{
			CombinedLightColor += Falloff * Attenuation * (AmbientColor + DiffuseColor + SpecularColor);
		}
	}

	Color = vec4(CombinedLightColor + vec3(texture(u_Material.EmissionMap, v_TexCoords)), 1.0);
};
