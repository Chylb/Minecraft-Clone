#pragma once

#include <GL/glew.h>
#include <stb_image/stb_image.h>

#include <filesystem>
#include <unordered_map>

class Resources
{
public:
	static void Initialize();
	static GLuint GetBlockTextureIx(const std::string& name);
	static GLuint GetTexture(const std::string& name);

private:
	inline static GLuint s_blockArrayTexture;
	inline static std::unordered_map<std::string, GLuint> s_blockTextureMap;
	inline static std::unordered_map<std::string, GLuint> s_textureMap;

	static void LoadBlockTextures();
	static void LoadTextures();
};
