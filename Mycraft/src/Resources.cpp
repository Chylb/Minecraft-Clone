#include "Resources.h"

void Resources::Initialize()
{
	LoadBlockTextures();
	LoadTextures();
}

GLuint Resources::GetBlockTextureIx(const std::string& name)
{
	auto mapIt = s_blockTextureMap.find(name);
	if (mapIt != s_blockTextureMap.end())
		return mapIt->second;

	printf("Tried to get nonexistent block texture %s \n", name.c_str());
	return s_blockTextureMap.at("missing_texture");
}

GLuint Resources::GetTexture(const std::string& name)
{
	return s_textureMap.at(name);
}

void Resources::LoadBlockTextures()
{
	stbi_set_flip_vertically_on_load(true);

	glGenTextures(1, &s_blockArrayTexture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, s_blockArrayTexture);

	glTexStorage3D(GL_TEXTURE_2D_ARRAY,
		1,						//mipmaps
		GL_RGBA8,				//internal format
		16, 16,					//width, height
		64						//number of layers
	);

	unsigned int i = 0;
	for (const auto& entry : std::filesystem::directory_iterator("res/textures/block")) {
		int width, height, nrChannels;
		unsigned char* data = stbi_load(entry.path().string().c_str(), &width, &height, &nrChannels, 0);

		if (data) {
			s_blockTextureMap[entry.path().stem().string()] = i;

			glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
				0,					//mipmap number
				0, 0, i,			//xoffset, yoffset, zoffset
				16, 16, 1,			//width, height, depth
				GL_RGBA,			//format
				GL_UNSIGNED_BYTE,	//type
				data);				//data

			i++;
		}
		else
		{
			printf("Failed to load texture %s \n", entry.path().string().c_str());
		}
		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_NEAREST);
}

void Resources::LoadTextures()
{
	stbi_set_flip_vertically_on_load(false);

	for (const auto& entry : std::filesystem::recursive_directory_iterator("res/textures/")) {
		if (entry.is_directory())
			continue;

		int width, height, nrChannels;
		unsigned char* data = stbi_load(entry.path().string().c_str(), &width, &height, &nrChannels, 0);

		if (data)
		{
			GLuint texId;
			glGenTextures(1, &texId);
			glBindTexture(GL_TEXTURE_2D, texId);
			// set the texture wrapping/filtering options (on the currently bound texture object)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			// load and generate the texture

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			s_textureMap[entry.path().stem().string()] = texId;
		}
		else
		{
			printf("Failed to load texture %s \n", entry.path().string().c_str());
		}
		stbi_image_free(data);
	}
}
