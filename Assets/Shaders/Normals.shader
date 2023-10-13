//!shader vertex
#version 460 core

layout (location = 0) in vec4 Position;
layout (location = 1) in vec3 Normal;

out VS_OUT {
	vec3 v_Normal;
} vs_out;

uniform mat4 u_ViewModel;
uniform mat3 u_NormalMatrix;

void main() {
	vs_out.v_Normal = u_NormalMatrix * Normal;
	gl_Position = u_ViewModel * Position;
};

//!shader geometry
#version 460 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
	vec3 v_Normal;
} gs_in[];

uniform mat4 u_Projection;

const float MAGNITUDE = 0.4;

void main() {
	for (int i = 0; i < 3; ++i)
	{
		gl_Position = u_Projection * gl_in[i].gl_Position;
		EmitVertex();
		gl_Position = u_Projection * (gl_in[i].gl_Position +
		vec4(gs_in[i].v_Normal, 0.0) * MAGNITUDE);
		EmitVertex();
		EndPrimitive();
	}
}

//!shader fragment
#version 460 core

out vec4 Color;

void main() {
	Color = vec4(1.0, 1.0, 0.0, 1.0);
};
