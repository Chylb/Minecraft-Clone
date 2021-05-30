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
	Block(bool opaque, StateContainer<Block, BlockState>::Builder builder);
	Block(bool opaque);

	bool IsOpaque() const;

	const StateContainer<Block, BlockState>& GetStateDefinition() const;
	BlockState* DefaultBlockState() const;
	virtual void Tick(World& world, BlockPos pos) const;

private:
	StateContainer<Block, BlockState> m_stateDefinition;
	BlockState* m_defaultBlockState;
	bool m_opaque;

	friend class BlockRegistry;
};
