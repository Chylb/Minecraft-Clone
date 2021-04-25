#pragma once

#include <iostream>
#include <chrono>
#include <string>

struct Timer
{
	std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
	std::chrono::duration<float> duration;
	std::string message;
	bool ticking;
	float threshold;

	Timer(std::string message, float threshold = 0)
	{
		this->message = message;
		this->threshold = threshold;
		begin();
	}

	~Timer()
	{
		finish();
	}

	inline void finish()
	{
		end = std::chrono::high_resolution_clock::now();
		duration = end - start;

		float ms = duration.count() * 1000.0f;
		if (ms > threshold)
			std::cout << message << ": " << ms << " ms" << std::endl;

		ticking = false;
	}

	inline void begin()
	{
		ticking = true;
		start = std::chrono::high_resolution_clock::now();
	}
};