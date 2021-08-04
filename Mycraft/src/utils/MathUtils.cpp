#include "MathUtils.h"

#include <algorithm>

float MathUtils::frac(float x)
{
	return x - floor(x);
}

int  MathUtils::signum(float x)
{
	if (x == 0.0)
		return 0;
	else
		return x > 0.0 ? 1 : -1;
}

float  MathUtils::clamp(float x, float min, float max)
{
	return std::max(std::min(x, max), min);
}