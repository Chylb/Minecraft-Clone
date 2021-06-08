#include "BlockStateProperties.h"

void BlockStateProperties::Initialize()
{
	axis = new Property<Direction::Axis>{ {Direction::Axis::x, Direction::Axis::y, Direction::Axis::z} };
	slabType = new Property<SlabType>{ {SlabType::bottom, SlabType::top, SlabType::full} };
}