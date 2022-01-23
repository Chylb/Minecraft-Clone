#pragma once

#include <GL/glew.h>
#include "glm/vec2.hpp"

#include <filesystem>
#include <unordered_map>

struct TextureAtlasSprite
{
	glm::vec2 uv0;
	glm::vec2 uv1;
};

class TextureAtlas
{
public:
	TextureAtlas();
	void Load(const std::string& path, bool padded);


	const TextureAtlasSprite& GetSprite(const std::string& spriteName) const;
	void Bind() const;

private:
	std::unordered_map<std::string, TextureAtlasSprite> m_sprites;
	GLuint m_id;
};
