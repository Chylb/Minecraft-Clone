#include "Block.h"

Block::Block(GLuint topTex, GLuint sideTex, GLuint bottomTex, bool opaque) :
	m_texTopIx(topTex),
	m_texSideIx(sideTex),
	m_texBottomIx(bottomTex),
	m_opaque(opaque)
{
}

Block::Block(GLuint tex, bool opaque) :
	m_texTopIx(tex),
	m_texSideIx(tex),
	m_texBottomIx(tex),
	m_opaque(opaque)
{
}

Block::Block() :
	m_opaque(true)
{
}

uint16_t Block::GetId() const
{
	return m_id;
}

bool Block::IsOpaque() const
{
	return m_opaque;
}

#pragma warning(disable:4838)
#pragma warning(disable:4244)

void Block::WriteTopFace(std::vector<float>& target, int world_x, int world_y, int world_z) const
{
	float vertices[] = {
		world_x + 0, world_y + 1, world_z + 0, 0, 1, m_texTopIx,
		world_x + 1, world_y + 1, world_z + 0, 1, 1, m_texTopIx,
		world_x + 1, world_y + 1, world_z + 1, 1, 0, m_texTopIx,
		world_x + 1, world_y + 1, world_z + 1, 1, 0, m_texTopIx,
		world_x + 0, world_y + 1, world_z + 1, 0, 0, m_texTopIx,
		world_x + 0, world_y + 1, world_z + 0, 0, 1, m_texTopIx
	};
	target.insert(target.end(), vertices, vertices + sizeof(vertices) / sizeof(float));
}

void Block::WriteNorthFace(std::vector<float>& target, int world_x, int world_y, int world_z) const
{
	float vertices[] = {
		world_x + 0, world_y + 0, world_z + 0, 1, 0, m_texSideIx,
		world_x + 1, world_y + 0, world_z + 0, 0, 0, m_texSideIx,
		world_x + 1, world_y + 1, world_z + 0, 0, 1, m_texSideIx,
		world_x + 1, world_y + 1, world_z + 0, 0, 1, m_texSideIx,
		world_x + 0, world_y + 1, world_z + 0, 1, 1, m_texSideIx,
		world_x + 0, world_y + 0, world_z + 0, 1, 0, m_texSideIx
	};
	target.insert(target.end(), vertices, vertices + sizeof(vertices) / sizeof(float));
}

void Block::WriteEastFace(std::vector<float>& target, int world_x, int world_y, int world_z) const
{
	float vertices[] = {
		world_x + 1, world_y + 1, world_z + 1, 0, 1, m_texSideIx,
		world_x + 1, world_y + 1, world_z + 0, 1, 1, m_texSideIx,
		world_x + 1, world_y + 0, world_z + 0, 1, 0, m_texSideIx,
		world_x + 1, world_y + 0, world_z + 0, 1, 0, m_texSideIx,
		world_x + 1, world_y + 0, world_z + 1, 0, 0, m_texSideIx,
		world_x + 1, world_y + 1, world_z + 1, 0, 1, m_texSideIx
	};
	target.insert(target.end(), vertices, vertices + sizeof(vertices) / sizeof(float));
}

void Block::WriteSouthFace(std::vector<float>& target, int world_x, int world_y, int world_z) const
{
	float vertices[] = {
		world_x + 0, world_y + 0, world_z + 1, 0, 0, m_texSideIx,
		world_x + 1, world_y + 0, world_z + 1, 1, 0, m_texSideIx,
		world_x + 1, world_y + 1, world_z + 1, 1, 1, m_texSideIx,
		world_x + 1, world_y + 1, world_z + 1, 1, 1, m_texSideIx,
		world_x + 0, world_y + 1, world_z + 1, 0, 1, m_texSideIx,
		world_x + 0, world_y + 0, world_z + 1, 0, 0, m_texSideIx
	};
	target.insert(target.end(), vertices, vertices + sizeof(vertices) / sizeof(float));
}

void Block::WriteWestFace(std::vector<float>& target, int world_x, int world_y, int world_z) const
{
	float vertices[] = {
		world_x + 0, world_y + 1, world_z + 1, 1, 1, m_texSideIx,
		world_x + 0, world_y + 1, world_z + 0, 0, 1, m_texSideIx,
		world_x + 0, world_y + 0, world_z + 0, 0, 0, m_texSideIx,
		world_x + 0, world_y + 0, world_z + 0, 0, 0, m_texSideIx,
		world_x + 0, world_y + 0, world_z + 1, 1, 0, m_texSideIx,
		world_x + 0, world_y + 1, world_z + 1, 1, 1, m_texSideIx
	};
	target.insert(target.end(), vertices, vertices + sizeof(vertices) / sizeof(float));
}

void Block::WriteBottomFace(std::vector<float>& target, int world_x, int world_y, int world_z) const
{
	float vertices[] = {
		world_x + 0, world_y + 0, world_z + 0, 1, 0, m_texBottomIx,
		world_x + 1, world_y + 0, world_z + 0, 0, 0, m_texBottomIx,
		world_x + 1, world_y + 0, world_z + 1, 0, 1, m_texBottomIx,
		world_x + 1, world_y + 0, world_z + 1, 0, 1, m_texBottomIx,
		world_x + 0, world_y + 0, world_z + 1, 1, 1, m_texBottomIx,
		world_x + 0, world_y + 0, world_z + 0, 1, 0, m_texBottomIx
	};
	target.insert(target.end(), vertices, vertices + sizeof(vertices) / sizeof(float));
}

#pragma warning(default:4838)
#pragma warning(default:4244)