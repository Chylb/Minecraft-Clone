#pragma once

namespace MathUtils
{
	float frac(float x)
	{
		return x - floor(x);
	}

	int signum(float x)
	{
		if (x == 0.0)
			return 0;
		else
			return x > 0.0 ? 1 : -1;
	}
}