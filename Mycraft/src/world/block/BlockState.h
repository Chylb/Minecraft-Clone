#pragma once

#include "../../state/StateHolder.h"

class Block;

class BlockState : public StateHolder<Block, BlockState>
{
public:
	BlockState() : StateHolder<Block, BlockState>() {}

	uint16_t GetId() const;
	const Block& GetBlock() const;

private:
	uint16_t m_id;

	friend class BlockRegistry;
};
