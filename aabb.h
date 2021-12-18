#pragma once

#include "rtweekend.h"

class aabb
{
public:
	aabb() {}
	aabb(const point3& min_point_, const point3& max_point_) : min_point(min_point_), max_point(max_point_) {}

	bool hit(const ray& r, float t_min, float t_max) const
	{
		for (int i = 0; i < 3; i++)
		{
			const float invD = 1.0 / r.dir[i];
			const float ta_min = (min_point[i] - r.origin[i]) * invD;
			const float ta_max = (max_point[i] - r.origin[i]) * invD;
			const float t0 = min(ta_min, ta_max);
			const float t1 = max(ta_min, ta_max);
			t_min = max(t0, t_min);
			t_max = min(t1, t_max);
			if(t_max <= t_min)
				return false;
		}
		return true;
	}

	point3 min_point, max_point;
};

aabb surrounding_box(const aabb& box0, const aabb& box1)
{
	return aabb(min(box0.min_point, box1.min_point), max(box0.max_point, box1.max_point));
}