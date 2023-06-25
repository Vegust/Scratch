//!shader vertex
#version 460 core

layout (location = 0) in vec4 position;

uniform mat4 u_MVP;

void main() {
	gl_Position = u_MVP * position;
};

//!shader fragment
#version 460 core

out vec4 Color;
uniform vec4 u_LightColor;

void main() {
	Color = u_LightColor;
};
