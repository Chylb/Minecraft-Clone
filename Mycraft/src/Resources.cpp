#include "Resources.h"

void Resources::LoadTextures()
{
	glGenTextures(1, &s_arrayTexture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, s_arrayTexture);

	glTexStorage3D(GL_TEXTURE_2D_ARRAY,
		1,						//mipmaps
		GL_RGB8,				//internal format
		16, 16,					//width, height
		64						//number of layers
	);

	unsigned int i = 0;
	for (const auto& entry : std::filesystem::directory_iterator("res/textures/")) {
		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(entry.path().string().c_str(), &width, &height, &nrChannels, 0);

		s_textureIxMap[entry.path().stem().string()] = i;

		glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
			0,					//mipmap number
			0, 0, i,			//xoffset, yoffset, zoffset
			16, 16, 1,			//width, height, depth
			GL_RGBA,			//format
			GL_UNSIGNED_BYTE,	//type
			data);				//data

		i++;
	}

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_NEAREST);
}

GLuint Resources::GetTextureIx(const std::string& name)
{
	auto mapIt = s_textureIxMap.find(name);
	if (mapIt != s_textureIxMap.end())
		return mapIt->second;

	return s_textureIxMap.at("missing_texture");
}
