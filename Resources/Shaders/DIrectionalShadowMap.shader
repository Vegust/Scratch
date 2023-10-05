//!shader vertex
#version 460 core

layout (location = 0) in vec4 Position;

uniform mat4 u_Model;
uniform mat4 u_ShadowMatrix;

void main() {
	gl_Position = u_ShadowMatrix * u_Model * Position;
};

//!shader fragment
#version 460 core
void main() {
	// this is done automatically:
	// gl_FragDepth = gl_FragCoord.z;
};