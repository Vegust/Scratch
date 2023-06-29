//
// Created by Vegust on 29.06.2023.
//

#include "cubemap.h"

#include "glad/glad.h"

cubemap::~cubemap()
{
	if (RendererId != 0)
	{
		
	}
}

cubemap::cubemap(cubemap&& InCubemap) noexcept
{
	RendererId = InCubemap.RendererId;
	InCubemap.RendererId = 0;
}

cubemap& cubemap::operator=(cubemap&& InCubemap) noexcept
{
	if (RendererId != 0)
	{
		
	}
	RendererId = InCubemap.RendererId;
	InCubemap.RendererId = 0;
	return *this;
}

void cubemap::Load()
{
	glGenTextures(1, &RendererId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, RendererId);
	
	int width, height, nrChannels;
	unsigned char *data;  
	for(unsigned int i = 0; i < textures_faces.size(); i++)
	{
		data = stbi_load(textures_faces[i].c_str(), &width, &height, &nrChannels, 0);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
			0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
		);
	}
	
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); 
}
