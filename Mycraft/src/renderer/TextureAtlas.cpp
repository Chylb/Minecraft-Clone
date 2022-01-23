#include "TextureAtlas.h"

#include "../utils/Image.h"
#include "MipmapGenerator.h"

TextureAtlas::TextureAtlas()
{
}

void TextureAtlas::Load(const std::string& path, bool padded)
{
	std::unordered_map<std::string, Image*> images;

	for (const auto& entry : std::filesystem::directory_iterator(path)) {
		auto texturePath = entry.path().string();
		auto textureName = entry.path().stem().string();

		auto image = new Image(texturePath);
		images[textureName] = image;
	}

	double atlasSideNonPow2 = ceil(sqrt(images.size()));
	double atlasSideLog = log2(atlasSideNonPow2);
	int atlasSide = pow(2, ceil(atlasSideLog)); //how many subtextures in one row

	constexpr int textureWidth = 16;
	int atlasSideLength = atlasSide * textureWidth;
	if (padded)
		atlasSideLength *= 3;

	Image* mipmap0 = new Image(atlasSideLength, atlasSideLength);
	if (!padded) {
		int i = 0; int j = 0; // i = column, j = row
		for (auto& [texName, image] : images)
		{
			int dstX0 = i * textureWidth;
			int dstY0 = j * textureWidth;
			int dstX1 = (i + 1) * textureWidth;
			int dstY1 = (j + 1) * textureWidth;
			mipmap0->Put(image, dstX0, dstY0);
			m_sprites[texName] = { {(float)dstX0 / atlasSideLength,(float)dstY0 / atlasSideLength},{(float)dstX1 / atlasSideLength,(float)dstY1 / atlasSideLength} };

			i++;
			if (i == atlasSide)
			{
				i = 0;
				j++;
			}
		}
	}
	else {
		int i = 0; int j = 0; // i = column, j = row
		for (auto& [texName, image] : images)
		{
			int dstX0 = 3 * i * textureWidth;
			int dstY0 = 3 * j * textureWidth;
			int dstX1 = (3 * i + 1) * textureWidth;
			int dstY1 = (3 * j + 1) * textureWidth;
			int dstX2 = (3 * i + 2) * textureWidth;
			int dstY2 = (3 * j + 2) * textureWidth;
			m_sprites[texName] = { {(float)dstX1 / atlasSideLength,(float)dstY1 / atlasSideLength},{(float)dstX2 / atlasSideLength,(float)dstY2 / atlasSideLength} };

			for (int ii = 0; ii < 3; ii++)
				for (int jj = 0; jj < 3; jj++) {
					Image transformedImage{ image };
					if (ii % 2 == 0) transformedImage.FlipX();
					if (jj % 2 == 0) transformedImage.FlipY();

					mipmap0->Put(&transformedImage, dstX0 + ii * textureWidth, dstY0 + jj * textureWidth);
				}

			i++;
			if (i == atlasSide)
			{
				i = 0;
				j++;
			}
		}
	}
	m_sprites["_whole"] = { {0,0},{1,1} };

	auto atlasMipmaps = MipmapGenerator::GenerateMipLevels(mipmap0, 4);

	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);

	for (int level = 0; level <= 4; level++)
	{
		glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, atlasSideLength >> level, atlasSideLength >> level, 0, GL_RGBA, GL_UNSIGNED_BYTE, atlasMipmaps[level]->GetData());
	}
	
	for (auto& [texName, image] : images)
		delete image;

	for (auto image : atlasMipmaps)
		delete image;
}

const TextureAtlasSprite& TextureAtlas::GetSprite(const std::string& spriteName) const
{
	return m_sprites.at(spriteName);
}

void TextureAtlas::Bind() const
{
	glBindTexture(GL_TEXTURE_2D, m_id);
}
