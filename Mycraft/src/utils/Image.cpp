#include "Image.h"

#include "stb_image/stb_image.h"

#include <stdexcept>

Image::Image(const std::string& path)
{
	int channels;
	m_data = stbi_load(path.c_str(), &m_width, &m_height, &channels, 0);

	if (m_data == nullptr)
		throw std::exception("ERROR: file does not exist");

	if (channels != c_channels)
		throw std::exception("ERROR: not supported number of channels");
}

Image::Image(int width, int height)
{
	m_data = new unsigned char[width * height * c_channels];
	m_width = width;
	m_height = height;
}

Image::Image(Image* image)
{
	m_width = image->GetWidth();
	m_height = image->GetHeight();
	m_data = new unsigned char[m_width * m_height * c_channels];

	for (int i = 0; i < m_width * m_height * c_channels; i++) {
		m_data[i] = image->GetData()[i];
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

unsigned char* Image::GetData()
{
	return m_data;
}

glm::u8vec4& Image::GetTexel(int x, int y)
{
	return *(glm::u8vec4*)&m_data[y * m_width * c_channels + x * c_channels];
}

void Image::Put(Image* image, int x, int y)
{
	for (int srcX = 0; srcX < image->GetWidth(); srcX++)
		for (int srcY = 0; srcY < image->GetHeight(); srcY++)
		{
			GetTexel(x + srcX, y + srcY) = image->GetTexel(srcX, srcY);
		}
}

void Image::FlipX()
{
	for (int x = 0; x < m_width / 2; x++)
		for (int y = 0; y < m_height; y++) {
			auto temp = GetTexel(x, y);
			GetTexel(x, y) = GetTexel(m_width - x - 1, y);
			GetTexel(m_width - x - 1, y) = temp;
		}
}

void Image::FlipY()
{
	for (int x = 0; x < m_width; x++)
		for (int y = 0; y < m_height / 2; y++) {
			auto temp = GetTexel(x, y);
			GetTexel(x, y) = GetTexel(x, m_height - y - 1);
			GetTexel(x, m_height - y - 1) = temp;
		}
}

Image::~Image()
{
	delete m_data;
}
