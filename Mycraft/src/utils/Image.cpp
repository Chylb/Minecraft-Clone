#include "Image.h"

#include "stb_image/stb_image.h"
#include <glm/ext/vector_int2.hpp>
#include <glm/ext/vector_uint4_sized.hpp>

#include <stdexcept>

Image::Image(const std::string& path)
{
	int channels;
	m_data = stbi_load(path.c_str(), &m_width, &m_height, &channels, 0);

	if (m_data == nullptr)
		throw std::exception("ERROR: file does not exist");

	if (channels == c_channels) {
	}
	else if (channels == 2) {
		unsigned char* convertedData = new unsigned char[m_width * m_height * c_channels];

		for (auto t = 0; t < m_width * m_height; t++) {
			auto value = m_data[t * 2];
			auto alpha = m_data[t * 2 + 1];
			convertedData[t * 4] = value;
			convertedData[t * 4 + 1] = value;
			convertedData[t * 4 + 2] = value;
			convertedData[t * 4 + 3] = alpha;
		}
		delete[] m_data;
		m_data = convertedData;
	}
	else {
		throw std::exception("ERROR: not supported number of channels");
	}
}

Image::Image(int width, int height)
{
	m_data = new unsigned char[width * height * c_channels];
	m_width = width;
	m_height = height;

	for (int i = 0; i < width * height * c_channels; i++) {
		m_data[i] = 0;
	}
}

Image::Image(const Image& image)
{
	m_width = image.GetWidth();
	m_height = image.GetHeight();
	m_data = new unsigned char[m_width * m_height * c_channels];

	for (int i = 0; i < m_width * m_height * c_channels; i++) {
		m_data[i] = image.GetData()[i];
	}
}

int Image::GetWidth() const
{
	return m_width;
}

int Image::GetHeight() const
{
	return m_height;
}

unsigned char* Image::GetData() const
{
	return m_data;
}

glm::u8vec4 Image::GetTexel(int x, int y) const
{
	return *(glm::u8vec4*)&m_data[y * m_width * c_channels + x * c_channels];
}

void Image::SetTexel(int x, int y, glm::u8vec4 texel)
{
	*(glm::u8vec4*)&m_data[y * m_width * c_channels + x * c_channels] = texel;
}

void Image::Put(const Image& srcImage, glm::uvec2 dstPos)
{
	Put(srcImage, { 0,0 }, dstPos, { srcImage.GetWidth(), srcImage.GetHeight() });
}

void Image::Put(const Image& srcImage, glm::uvec2 srcPos, glm::uvec2 dstPos, glm::uvec2 selectionSize)
{
	for (int x = 0; x < selectionSize.x; x++)
		for (int y = 0; y < selectionSize.y; y++)
		{
			SetTexel(dstPos.x + x, dstPos.y + y, srcImage.GetTexel(srcPos.x + x, srcPos.y + y));
		}
}

void Image::FlipX()
{
	for (int x = 0; x < m_width / 2; x++)
		for (int y = 0; y < m_height; y++) {
			auto temp = GetTexel(x, y);
			SetTexel(x, y, GetTexel(m_width - x - 1, y));
			SetTexel(m_width - x - 1, y, temp);
		}
}

void Image::FlipY()
{
	for (int x = 0; x < m_width; x++)
		for (int y = 0; y < m_height / 2; y++) {
			auto temp = GetTexel(x, y);
			SetTexel(x, y, GetTexel(x, m_height - y - 1));
			SetTexel(x, m_height - y - 1, temp);
		}
}

Image::~Image()
{
	delete[] m_data;
}
