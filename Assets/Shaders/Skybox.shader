//!shader vertex
#version 460 core

layout (location = 0) in vec3 Position;
out vec3 v_TexDirection;
uniform mat4 u_ProjectionView;
void main() {
	v_TexDirection = Position;
	gl_Position = (u_ProjectionView * vec4(Position, 1.f)).xyww;
};

//!shader fragment
#version 460 core

#define SKY_CUBEMAP_SLOT 11

out vec4 Color;
in vec3 v_TexDirection;

layout (binding = SKY_CUBEMAP_SLOT) uniform samplerCube u_Cubemap;

void main() {
	Color = texture(u_Cubemap, v_TexDirection);
};