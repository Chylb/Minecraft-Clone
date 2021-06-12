#include "BlockStateProperties.h"

void BlockStateProperties::Initialize()
{
	axis = new Property<Direction::Axis>{ {Direction::Axis::x, Direction::Axis::y, Direction::Axis::z} };
	slabType = new Property<SlabType>{ {SlabType::top, SlabType::bottom, SlabType::full} };
}