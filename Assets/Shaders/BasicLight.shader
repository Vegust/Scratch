//!shader vertex
#version 460 core

layout (location = 0) in vec4 position;

uniform mat4 u_MVP;

void main() {
	gl_Position = u_MVP * position;
};

//!shader fragment
#version 460 core

struct light {
	vec3 Position;
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
};

out vec4 Color;

uniform light u_Light;

void main() {
	Color = vec4(u_Light.Diffuse, 1.0);
};
