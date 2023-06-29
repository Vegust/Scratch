//!shader vertex
#version 460 core

layout (location = 0) in vec3 Position;
out vec3 v_TexDirection;
uniform mat4 u_ProjectionView;
void main() {
	v_TexDirection = Position;
	gl_Position = u_ProjectionView * vec4(Position, 1.f);
};

//!shader fragment
#version 460 core

out vec4 Color;
in vec3 v_TexDirection;
uniform samplerCube u_Cubemap;

void main() {
	Color = texture(u_Cubemap, v_TexDirection);
};