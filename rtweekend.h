#pragma once

#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>
#include <random>

// Disable annoying warnings
#pragma warning(disable : 4305) // Disable truncation from double to float
#pragma warning(disable : 4244) // Disable conversion from double to float

// Usings

using std::shared_ptr;
using std::make_shared;
using std::sqrt;

// Constants

const float infinity = std::numeric_limits<float>::infinity();
const float pi = 3.1415926535897932385;

// Utility functions

inline float degrees_to_radians(float degrees)
{
	return degrees * pi / 180.0;
}

std::mt19937& get_generator()
{
	static thread_local std::mt19937 generator;
	return generator;
}

inline float random_float(float min, float max)
{
	// returns a random real in [min,max)
	std::uniform_real_distribution<float> distribution(min, max);
	return distribution(get_generator());
}

inline float random_float()
{
	return random_float(0, 1);
}

inline int random_int(int min, int max)
{
	// returns a random integer in [min,max]
	std::uniform_int_distribution<> distribution(min, max+1);
	return distribution(get_generator());
}

inline float min(float x, float y)
{
	return x < y ? x : y;
}

inline float max(float x, float y)
{
	return x > y ? x : y;
}

inline float clamp(float x, float min, float max)
{
	if(x < min) return min;
	if(x > max) return max;
	return x;
}

// Common headers

#include "ray.h"
#include "vec3.h"