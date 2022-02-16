#pragma once

#include <vector>

#include "../utils/Image.h"

namespace MipmapGenerator
{
	std::vector<Image> GenerateMipLevels(const Image& image, int levels);
}
