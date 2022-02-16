#include "Resources.h"

void Resources::Initialize()
{
	stbi_set_flip_vertically_on_load(true);
	s_blockTextureAtlas.Load("res/textures/block", true);
	stbi_set_flip_vertically_on_load(false);
	LoadTextures();
}

TextureAtlas& Resources::GetBlockTextureAtlas()
{
	return s_blockTextureAtlas;
}

GLuint Resources::GetTexture(const std::string& name)
{
	auto mapIt = s_textureMap.find(name);
	if (mapIt != s_textureMap.end())
		return mapIt->second;

	printf("Tried to get nonexistent texture %s \n", name.c_str());
	return s_textureMap.at("missing_texture");
}

void Resources::LoadTextures()
{
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
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			// set texture filtering parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			// load and generate the texture

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			s_textureMap[entry.path().stem().string()] = texId;
		}
		else
		{
			printf("Failed to load texture %s \n", entry.path().string().c_str());
		}
		stbi_image_free(data);
	}
}
