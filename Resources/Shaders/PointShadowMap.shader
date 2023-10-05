//!shader vertex
#version 460 core
layout (location = 0) in vec4 Position;
uniform mat4 u_Model;
uniform vec3 u_LightPos;
void main() {
	gl_Position = u_Model * Position - vec4(u_LightPos,0);
};

//!shader geometry
#version 460 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;
uniform mat4 u_PointLightViews[6];
uniform float u_Attenuation;
out vec4 g_FragPos;
void main() {
	for (int Face = 0; Face < 6; ++Face)
	{
		gl_Layer = Face;
		for (int i = 0; i < 3; ++i)
		{
			g_FragPos = gl_in[i].gl_Position / u_Attenuation;
			gl_Position = u_PointLightViews[Face] * g_FragPos;
			EmitVertex();
		}
		EndPrimitive();
	}
}

//!shader fragment
#version 460 core
in vec4 g_FragPos;
void main() {
	gl_FragDepth = length(g_FragPos.xyz);
};