#include "BlockStateProperties.h"

void BlockStateProperties::Initialize()
{
	axis = new Property<Axis>{ {Axis::x, Axis::y, Axis::z} };
}