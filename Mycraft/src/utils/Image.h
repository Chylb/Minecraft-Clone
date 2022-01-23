#pragma once

#include <string>
#include <glm/ext/vector_uint4_sized.hpp>

class Image
{
public:
	Image(const std::string& path);
	Image(int width, int height);
	Image(Image* image);

	int GetWidth() const;
	int GetHeight() const;
	unsigned char* GetData();
	glm::u8vec4& GetTexel(int x, int y);
	void Put(Image* image, int x, int y);
	void FlipX();
	void FlipY();
	~Image();

private:
	int m_width;
	int m_height;
	unsigned char* m_data;

	static constexpr int c_channels = 4;
};
