#pragma once

#include "vec3.h"

class ray
{
public:
	ray() {}
	ray(const point3& origin_, const vec3& dir_) : origin(origin_), dir(dir_)
	{}

	point3 at(double t) const
	{
		return orig + t*dir;
	}

	point3 origin;
	vec3 direction;
};