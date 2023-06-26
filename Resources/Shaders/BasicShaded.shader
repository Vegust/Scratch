//!shader vertex
#version 460 core

layout (location = 0) in vec4 Position;
layout (location = 1) in vec3 Normal;

out vec3 v_Normal;
out vec3 v_FragPos;

uniform mat4 u_MVP;
uniform mat4 u_ViewModel;
uniform mat3 u_NormalMatrix;

void main() {
	v_Normal = u_NormalMatrix * Normal;
	v_FragPos = vec3(u_ViewModel * Position);
	gl_Position = u_MVP * Position;
};

//!shader fragment
#version 460 core

struct material {
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
	float Shininess;
};

struct light {
	vec3 Position;
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
};

in vec3 v_Normal;
in vec3 v_FragPos;

out vec4 Color;

uniform material u_Material;
uniform light u_Light;

void main() {
	vec3 AmbientColor = u_Light.Ambient * u_Material.Ambient;

	vec3 NormalizedNormal = normalize(v_Normal);
	vec3 LightDirection = normalize(u_Light.Position - v_FragPos);
	float DiffuseImpact = max(dot(NormalizedNormal, LightDirection), 0.0);
	vec3 DiffuseColor = DiffuseImpact * u_Light.Diffuse * u_Material.Diffuse;

	vec3 ReflectDirection = reflect(-LightDirection, v_Normal);
	vec3 ViewDirection = normalize(-v_FragPos);
	float SpecularImpact = pow(max(dot(ViewDirection, ReflectDirection), 0.0), u_Material.Shininess);
	vec3 SpecularColor = u_Light.Specular * u_Material.Specular * SpecularImpact;

	Color = vec4(DiffuseColor + AmbientColor + SpecularColor, 1.0);
};
