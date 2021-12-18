#pragma once

#include "vec3.h"

class ray
{
public:
	ray() {}
	ray(const point3& origin_, const vec3& dir_, float time_ = 0.0)
		: origin(origin_), dir(dir_), time(time_)
	{}

	point3 at(float t) const
	{
		return origin + t*dir;
	}

	point3 origin;
	vec3 dir;
	float time;
};