#include "BlockState.h"

uint16_t BlockState::GetId() const
{
	return m_id;
}

const Block& BlockState::GetBlock() const
{
	return *m_owner;
}
