#pragma once

#include <GL/glew.h>

#include <vector>
#include <unordered_map>

#include "BlockPos.h"
#include "BlockState.h"
#include "../../state/Property.h"
#include "../state/StateContainer.h"

class World;

class Block
{
public:
	Block(GLuint topTex, GLuint sideTex, GLuint bottomTex, bool opaque, StateContainer<Block, BlockState>::Builder builder);
	Block(GLuint topTex, GLuint sideTex, GLuint bottomTex, bool opaque);
	Block(GLuint tex, bool opaque);
	Block();

	bool IsOpaque() const;

	template<Direction::Direction dir>
	void WriteFace(std::vector<float>& target, BlockPos pos) const;

	const StateContainer<Block, BlockState>& GetStateDefinition() const;
	BlockState* DefaultBlockState() const;
	virtual void Tick(World& world, BlockPos pos) const;


private:
	StateContainer<Block, BlockState> m_stateDefinition;
	BlockState* m_defaultBlockState;

	bool m_opaque;

	GLuint m_texTopIx;
	GLuint m_texSideIx;
	GLuint m_texBottomIx;

	friend class BlockRegistry;
};
