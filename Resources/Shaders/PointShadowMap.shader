//!shader vertex
#version 460 core

layout (location = 0) in vec4 Position;
uniform mat4 u_Model;
void main() {
	gl_Position = u_Model * Position;
};

//!shader geometry
#version 460 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

uniform mat4 u_PointLightViews[6];
out vec4 g_FragPos;

void main() {
	for (int Face = 0; Face < 6; ++Face)
	{
		gl_Layer = Face;
		for (int i = 0; i < 3; ++i)
		{
			g_FragPos = gl_in[i].gl_Position;
			gl_Position = u_PointLightViews[Face] * g_FragPos;
			EmitVertex();
		}
		EndPrimitive();
	}
}

//!shader fragment
#version 460 core

in vec4 g_FragPos;

uniform vec3 u_LightPos;
uniform float u_Attenuation;

void main() {
	float LightDistance = length(g_FragPos.xyz - u_LightPos);
	gl_FragDepth = LightDistance / u_Attenuation;
};