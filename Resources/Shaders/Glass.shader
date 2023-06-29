//!shader vertex
#version 460 core

layout (location = 0) in vec4 Position;
layout (location = 1) in vec3 Normal;

out vec3 v_Normal;
out vec3 v_FragPos;

uniform mat4 u_MVP;
uniform mat4 u_Model;
uniform mat3 u_NormalModelMatrix;

void main() {
	v_Normal = u_NormalModelMatrix * Normal;
	v_FragPos = vec3(u_Model * Position);
	gl_Position = u_MVP * Position;
};

//!shader fragment
#version 460 core

out vec4 Color;

in vec3 v_Normal;
in vec3 v_FragPos;

uniform vec3 u_CameraPos;
uniform samplerCube u_Cubemap;
uniform float u_RefractiveIndex;

void main() {
	float Ratio = 1.00 / u_RefractiveIndex;
	vec3 NegativeViewDirection = normalize(v_FragPos - u_CameraPos);
	vec3 ReflectedViewDirection = refract(NegativeViewDirection, normalize(v_Normal), Ratio);
	Color = vec4(texture(u_Cubemap, ReflectedViewDirection).rgb, 1.0);
};