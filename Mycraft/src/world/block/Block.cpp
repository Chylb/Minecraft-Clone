#include "Block.h"

#include "../World.h"

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
	m_opaque(false)
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

void Block::Tick(World& world, BlockPos pos)
{
}

#pragma warning(disable:4838)
#pragma warning(disable:4244)

template<>
void Block::WriteFace<Direction::north>(std::vector<float>& target, BlockPos pos) const
{
	float vertices[] = {
		pos.x + 1, pos.y + 1, pos.z + 0, 0, 1, m_texSideIx,
		pos.x + 1, pos.y + 0, pos.z + 0, 0, 0, m_texSideIx,
		pos.x + 0, pos.y + 0, pos.z + 0, 1, 0, m_texSideIx,
		pos.x + 0, pos.y + 0, pos.z + 0, 1, 0, m_texSideIx,
		pos.x + 0, pos.y + 1, pos.z + 0, 1, 1, m_texSideIx,
		pos.x + 1, pos.y + 1, pos.z + 0, 0, 1, m_texSideIx
	};
	target.insert(target.end(), vertices, vertices + sizeof(vertices) / sizeof(float));
}

template<>
void Block::WriteFace<Direction::east>(std::vector<float>& target, BlockPos pos) const
{
	float vertices[] = {
		pos.x + 1, pos.y + 1, pos.z + 1, 0, 1, m_texSideIx,
		pos.x + 1, pos.y + 0, pos.z + 1, 0, 0, m_texSideIx,
		pos.x + 1, pos.y + 0, pos.z + 0, 1, 0, m_texSideIx,
		pos.x + 1, pos.y + 0, pos.z + 0, 1, 0, m_texSideIx,
		pos.x + 1, pos.y + 1, pos.z + 0, 1, 1, m_texSideIx,
		pos.x + 1, pos.y + 1, pos.z + 1, 0, 1, m_texSideIx
	};
	target.insert(target.end(), vertices, vertices + sizeof(vertices) / sizeof(float));
}

template<>
void Block::WriteFace<Direction::south>(std::vector<float>& target, BlockPos pos) const
{
	float vertices[] = {
		pos.x + 0, pos.y + 0, pos.z + 1, 0, 0, m_texSideIx,
		pos.x + 1, pos.y + 0, pos.z + 1, 1, 0, m_texSideIx,
		pos.x + 1, pos.y + 1, pos.z + 1, 1, 1, m_texSideIx,
		pos.x + 1, pos.y + 1, pos.z + 1, 1, 1, m_texSideIx,
		pos.x + 0, pos.y + 1, pos.z + 1, 0, 1, m_texSideIx,
		pos.x + 0, pos.y + 0, pos.z + 1, 0, 0, m_texSideIx
	};
	target.insert(target.end(), vertices, vertices + sizeof(vertices) / sizeof(float));
}

template<>
void Block::WriteFace<Direction::west>(std::vector<float>& target, BlockPos pos) const
{
	float vertices[] = {
		pos.x + 0, pos.y + 1, pos.z + 1, 1, 1, m_texSideIx,
		pos.x + 0, pos.y + 1, pos.z + 0, 0, 1, m_texSideIx,
		pos.x + 0, pos.y + 0, pos.z + 0, 0, 0, m_texSideIx,
		pos.x + 0, pos.y + 0, pos.z + 0, 0, 0, m_texSideIx,
		pos.x + 0, pos.y + 0, pos.z + 1, 1, 0, m_texSideIx,
		pos.x + 0, pos.y + 1, pos.z + 1, 1, 1, m_texSideIx
	};
	target.insert(target.end(), vertices, vertices + sizeof(vertices) / sizeof(float));
}

template<>
void Block::WriteFace<Direction::top>(std::vector<float>& target, BlockPos pos) const
{
	float vertices[] = {
		pos.x + 0, pos.y + 1, pos.z + 0, 0, 1, m_texTopIx,
		pos.x + 0, pos.y + 1, pos.z + 1, 0, 0, m_texTopIx,
		pos.x + 1, pos.y + 1, pos.z + 1, 1, 0, m_texTopIx,
		pos.x + 1, pos.y + 1, pos.z + 1, 1, 0, m_texTopIx,
		pos.x + 1, pos.y + 1, pos.z + 0, 1, 1, m_texTopIx,
		pos.x + 0, pos.y + 1, pos.z + 0, 0, 1, m_texTopIx
	};
	target.insert(target.end(), vertices, vertices + sizeof(vertices) / sizeof(float));
}

template<>
void Block::WriteFace<Direction::bottom>(std::vector<float>& target, BlockPos pos) const
{
	float vertices[] = {
		pos.x + 0, pos.y + 0, pos.z + 0, 0, 0, m_texBottomIx,
		pos.x + 1, pos.y + 0, pos.z + 0, 1, 0, m_texBottomIx,
		pos.x + 1, pos.y + 0, pos.z + 1, 1, 1, m_texBottomIx,
		pos.x + 1, pos.y + 0, pos.z + 1, 1, 1, m_texBottomIx,
		pos.x + 0, pos.y + 0, pos.z + 1, 0, 1, m_texBottomIx,
		pos.x + 0, pos.y + 0, pos.z + 0, 0, 0, m_texBottomIx
	};
	target.insert(target.end(), vertices, vertices + sizeof(vertices) / sizeof(float));
}

#pragma warning(default:4838)
#pragma warning(default:4244)