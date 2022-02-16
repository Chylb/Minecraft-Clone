#include "MathUtils.h"

#include <algorithm>

float MathUtils::frac(float x)
{
	return x - floor(x);
}

int MathUtils::signum(float x)
{
	if (x == 0.0)
		return 0;
	else
		return x > 0.0 ? 1 : -1;
}

float MathUtils::clamp(float x, float min, float max)
{
	return std::max(std::min(x, max), min);
}

int MathUtils::smallestEncompassingPowerOfTwo(int x)
{
	x -= 1;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	return x + 1;
}

void MathUtils::normalizePlane(glm::vec4& plane)
{
	float mag = sqrt(plane.x * plane.x + plane.y * plane.y + plane.z * plane.z);
	plane /= mag;
}

std::array<glm::vec4, 4> MathUtils::extractFrustumPlanes(const glm::mat4& frustum)
{
	std::array<glm::vec4, 4> planes;

	//Left clipping plane
	planes[0].x = frustum[0][3] + frustum[0][0];
	planes[0].y = frustum[1][3] + frustum[1][0];
	planes[0].z = frustum[2][3] + frustum[2][0];
	planes[0].w = frustum[3][3] + frustum[3][0];
	//Right clipping plane   					   
	planes[1].x = frustum[0][3] - frustum[0][0];
	planes[1].y = frustum[1][3] - frustum[1][0];
	planes[1].z = frustum[2][3] - frustum[2][0];
	planes[1].w = frustum[3][3] - frustum[3][0];
	//Top clipping plane	   					   
	planes[2].x = frustum[0][3] - frustum[0][1];
	planes[2].y = frustum[1][3] - frustum[1][1];
	planes[2].z = frustum[2][3] - frustum[2][1];
	planes[2].w = frustum[3][3] - frustum[3][1];
	//Bottom clipping plane   					   
	planes[3].x = frustum[0][3] + frustum[0][1];
	planes[3].y = frustum[1][3] + frustum[1][1];
	planes[3].z = frustum[2][3] + frustum[2][1];
	planes[3].w = frustum[3][3] + frustum[3][1];

	normalizePlane(planes[0]);
	normalizePlane(planes[1]);
	normalizePlane(planes[2]);
	normalizePlane(planes[3]);

	return planes;
}
