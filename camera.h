#pragma once

#include "rtweekend.h"

class camera
{
public:
	camera(
		point3 lookfrom,
		point3 lookat,
		vec3 vup,
		double vfov,
		double aspect_ratio)
	{
		const auto theta = degrees_to_radians(vfov);
		const auto h = tan(theta/2);
		const auto viewport_height = 2.0 * h;
		const auto viewport_width = aspect_ratio * viewport_height;

		const auto w = normalize(lookfrom-lookat);
		const auto u = normalize(cross(vup, w));
		const auto v = cross(w, u);

		origin = lookfrom;
		horizontal = viewport_width * u;
		vertical = viewport_height * v;
		lower_left_corner = origin - horizontal/2 - vertical/2 - w;
	}

	ray get_ray(double u, double v) const
	{
		return ray(origin, lower_left_corner + u*horizontal + v*vertical - origin);
	}

private:
	point3 origin;
	point3 lower_left_corner;
	vec3 horizontal;
	vec3 vertical;
};