#include "TextureAtlas.h"

#include <numeric>

#include "../utils/Image.h"
#include "MipmapGenerator.h"
#include "../utils/MathUtils.h"

TextureAtlas::TextureAtlas()
{
}

void TextureAtlas::Load(const std::string& path, bool padded)
{
	constexpr int textureWidth = 16;
	std::unordered_map<std::string, std::vector<Image>> textures;

	for (const auto& entry : std::filesystem::directory_iterator(path)) {
		auto texturePath = entry.path().string();
		auto textureName = entry.path().stem().string();

		textures[textureName] = LoadTexture(texturePath);
	}

	int atlasSide = MathUtils::smallestEncompassingPowerOfTwo( ceil( sqrt( textures.size()))); //how many subtextures in one row

	for (auto& [texName, image] : textures) {
		m_frameCount = std::lcm(image.size(), m_frameCount);
	}

	int atlasSideLength = atlasSide * textureWidth;
	if (padded)
		atlasSideLength *= 3;

	std::vector<Image> mipmap0s;
	for (int frame = 0; frame < m_frameCount; frame++) {
		Image mipmap0 = { atlasSideLength, atlasSideLength };
		if (!padded) {
			int i = 0; int j = 0; // i = column, j = row
			for (auto& [texName, texture] : textures)
			{
				int dstX0 = i * textureWidth;
				int dstY0 = j * textureWidth;
				int dstX1 = (i + 1) * textureWidth;
				int dstY1 = (j + 1) * textureWidth;
				mipmap0.Put(texture[frame % texture.size()], { dstX0, dstY0 });
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
			for (auto& [texName, texture] : textures)
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
						Image transformedImage{ texture[frame % texture.size()] };
						if (ii % 2 == 0) transformedImage.FlipX();
						if (jj % 2 == 0) transformedImage.FlipY();

						mipmap0.Put(transformedImage, { dstX0 + ii * textureWidth, dstY0 + jj * textureWidth });
					}

				i++;
				if (i == atlasSide)
				{
					i = 0;
					j++;
				}
			}
		}
		mipmap0s.push_back(mipmap0);
	}
	m_sprites["_whole"] = { {0,0},{1,1} };

	std::vector<std::vector<Image>> atlasMipmaps;
	for (auto frame = 0; frame < m_frameCount; frame++) {
		atlasMipmaps.push_back(MipmapGenerator::GenerateMipLevels(mipmap0s[frame], 4));
	}

	m_ids = new GLuint[m_frameCount];
	glGenTextures(m_frameCount, m_ids);

	for (auto frame = 0; frame < m_frameCount; frame++) {
		glBindTexture(GL_TEXTURE_2D, m_ids[frame]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);

		for (int level = 0; level <= 4; level++)
		{
			glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, atlasSideLength >> level, atlasSideLength >> level, 0, GL_RGBA, GL_UNSIGNED_BYTE, atlasMipmaps[frame][level].GetData());
		}
	}
}

const TextureAtlasSprite& TextureAtlas::GetSprite(const std::string& spriteName) const
{
	return m_sprites.at(spriteName);
}

void TextureAtlas::Tick()
{
	m_currentFrame = (m_currentFrame + 1) % m_frameCount;
}

void TextureAtlas::Bind() const
{
	glBindTexture(GL_TEXTURE_2D, m_ids[m_currentFrame]);
}

//returns vector of images because texture can be animated
std::vector<Image> TextureAtlas::LoadTexture(const std::string& path)
{
	auto srcImage = Image(path);
	std::vector<Image> animationImages;
	int frameCount = srcImage.GetHeight() / srcImage.GetWidth();

	for (int i = 0; i < frameCount; i++) {
		animationImages.push_back({ 16,16 });
		auto& image = animationImages[i];
		image.Put(srcImage, { 0, i * 16 }, { 0, 0 }, { 16, 16 });
	}

	return animationImages;
}
