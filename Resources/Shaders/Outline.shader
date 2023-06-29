//!shader vertex
#version 460 core

layout (location = 0) in vec4 Position;
layout (location = 1) in vec3 Normal;

uniform mat4 u_MVP;

void main() {
	gl_Position = u_MVP * (Position + vec4(Normal * 0.05, 0.f));
};

//!shader fragment
#version 460 core
out vec4 Color;

void main() {
	Color = vec4(0.28, 0.04, 0.26, 1.0);
};
