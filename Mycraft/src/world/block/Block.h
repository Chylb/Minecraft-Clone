#pragma once

#include <GL/glew.h>

#include <vector>
#include <unordered_map>

#include "BlockPos.h"

class Block
{
public:
	Block(GLuint topTex, GLuint sideTex, GLuint bottomTex, bool opaque);
	Block(GLuint tex, bool opaque);
	Block();

	uint16_t GetId() const;

	bool IsOpaque() const;

	template<Direction::Direction dir>
	void WriteFace(std::vector<float>& target, BlockPos pos) const;

private:
	uint16_t m_id;

	bool m_opaque;

	GLuint m_texTopIx;
	GLuint m_texSideIx;
	GLuint m_texBottomIx;

	friend class BlockRegistry;
};
