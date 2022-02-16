#pragma once

#include <GL/glew.h>
#include <stb_image/stb_image.h>

#include <filesystem>
#include <unordered_map>

#include "renderer/TextureAtlas.h"

class Resources
{
public:
	static void Initialize();
	static TextureAtlas& GetBlockTextureAtlas();
	static GLuint GetTexture(const std::string& name);

private:
	inline static TextureAtlas s_blockTextureAtlas;
	inline static std::unordered_map<std::string, GLuint> s_textureMap;

	static void LoadTextures();
};
