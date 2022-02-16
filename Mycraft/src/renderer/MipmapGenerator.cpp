#include "MipmapGenerator.h"

#include <glm/ext/vector_uint4_sized.hpp>

#include "../utils/Image.h"

std::vector<Image> MipmapGenerator::GenerateMipLevels(const Image& image, int levels)
{
	std::vector images{ image };

	for (int level = 1; level <= levels; level++)
	{
		Image& baseImage = images[level - 1];
		auto mipImage = Image(baseImage.GetWidth() >> 1, baseImage.GetHeight() >> 1);

		for (int x = 0; x < mipImage.GetWidth(); x++)
			for (int y = 0; y < mipImage.GetHeight(); y++)
			{
				auto t1 = baseImage.GetTexel(2 * x, 2 * y);
				auto t2 = baseImage.GetTexel(2 * x + 1, 2 * y);
				auto t3 = baseImage.GetTexel(2 * x, 2 * y + 1);
				auto t4 = baseImage.GetTexel(2 * x + 1, 2 * y + 1);

				mipImage.SetTexel(x, y, {
				 (t1.r + t2.r + t3.r + t4.r) / 4,
				 (t1.g + t2.g + t3.g + t4.g) / 4,
				 (t1.b + t2.b + t3.b + t4.b) / 4,
				 (t1.a + t2.a + t3.a + t4.a) / 4
					});
			}
		images.push_back(mipImage);
	}
	return images;
}
