//!shader vertex
#version 460 core

layout (location = 0) in vec2 Position;
layout (location = 1) in vec2 TexCoords;

out vec2 v_TexCoords;

void main() {
	gl_Position = vec4(Position.x, Position.y, 0.0, 1.0);
	v_TexCoords = TexCoords;
};

//!shader fragment
#version 460 core

in vec2 v_TexCoords;
out vec4 Color;

uniform sampler2D u_Buffer;
uniform bool u_Grayscale;
uniform float u_GammaCorrection;

void main() {
	vec4 TextureColor = texture(u_Buffer, v_TexCoords);
	TextureColor.rgb = pow(TextureColor.rgb, vec3(1.0 / u_GammaCorrection));
	if (u_Grayscale)
	{
		float average = 0.2126 * TextureColor.r + 0.7152 * TextureColor.g + 0.0722 * TextureColor.b;
		Color = vec4(average, average, average, 1.0);
	}
	else
	{
		Color = TextureColor;
	}
};