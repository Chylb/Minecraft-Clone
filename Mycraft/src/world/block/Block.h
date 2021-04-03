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

	void WriteTopFace(std::vector<float>& target, BlockPos pos) const;
	void WriteNorthFace(std::vector<float>& target, BlockPos pos) const;
	void WriteEastFace(std::vector<float>& target, BlockPos pos) const;
	void WriteSouthFace(std::vector<float>& target, BlockPos pos) const;
	void WriteWestFace(std::vector<float>& target, BlockPos pos) const;
	void WriteBottomFace(std::vector<float>& target, BlockPos pos) const;

private:
	uint16_t m_id;

	bool m_opaque;

	GLuint m_texTopIx;
	GLuint m_texSideIx;
	GLuint m_texBottomIx;

	friend class BlockRegistry;
};
