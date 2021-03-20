#pragma once

#include <GL/glew.h>
#include <stb_image/stb_image.h>

#include <filesystem>
#include <unordered_map>

class Resources
{
public:
	static void LoadTextures();
	static GLuint GetTextureIx(const std::string& name);

private:
	inline static GLuint s_arrayTexture;
	inline static std::unordered_map<std::string, GLuint> s_textureIxMap;
};
