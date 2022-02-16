#pragma once

#include <string>
#include <glm/fwd.hpp>
//#include <glm/ext/vector_uint4_sized.hpp>

class Image
{
public:
	Image(const std::string& path);
	Image(int width, int height);
	Image(const Image& image);

	int GetWidth() const;
	int GetHeight() const;
	unsigned char* GetData() const;
	glm::u8vec4 GetTexel(int x, int y) const;
	void SetTexel(int x, int y, glm::u8vec4 texel);
	void Put(const Image& srcImage, glm::uvec2 dstPos);
	void Put(const Image& srcImage, glm::uvec2 srcPos, glm::uvec2 dstPos, glm::uvec2 selectionSize);
	void FlipX();
	void FlipY();
	~Image();

private:
	int m_width;
	int m_height;
	unsigned char* m_data;

	static constexpr int c_channels = 4;
};
