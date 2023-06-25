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

in vec3 v_Normal;
in vec3 v_FragPos;

out vec4 Color;

uniform vec4 u_Color;
uniform vec4 u_LightColor;
uniform float u_AmbientStrength;
uniform vec3 u_LightPos;
uniform int u_SpecularPower;
uniform float u_SpecularStrength;

void main() {
	vec4 AmbientColor = u_LightColor * vec4(vec3(u_AmbientStrength), 1.0);

	vec3 NormalizedNormal = normalize(v_Normal);
	vec3 LightDirection = normalize(u_LightPos - v_FragPos);
	float DiffuseImpact = max(dot(NormalizedNormal, LightDirection), 0.0);
	vec4 DiffuseColor = DiffuseImpact * u_LightColor;

	vec3 ReflectDirection = reflect(-LightDirection, v_Normal);
	vec3 ViewDirection = normalize(-v_FragPos);
	float SpecularImpact = pow(max(dot(ViewDirection, ReflectDirection), 0.0), float(u_SpecularPower));
	vec4 SpecularColor = u_SpecularStrength * SpecularImpact * u_LightColor;

	Color = u_Color * (DiffuseColor + AmbientColor + SpecularColor);
};
