#pragma once

#include <glm/glm.hpp>
#include <array>

namespace MathUtils
{
	float frac(float x);
	int signum(float x);
	float clamp(float x, float min, float max);
	int smallestEncompassingPowerOfTwo(int x);
	std::array<glm::vec4, 4> extractFrustumPlanes(const glm::mat4& frustum);
	void normalizePlane(glm::vec4& plane);
}
