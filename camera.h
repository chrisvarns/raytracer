#pragma once

#include "rtweekend.h"

class camera
{
public:
	camera(
		point3 lookfrom,
		point3 lookat,
		vec3 vup,
		float vfov,
		float aspect_ratio,
		float aperture,
		float focus_dist,
		float time0_ = 0.f,
		float time1_ = 0.f)
	{
		const auto theta = degrees_to_radians(vfov);
		const auto h = tan(theta/2);
		const auto viewport_height = 2.0 * h;
		const auto viewport_width = aspect_ratio * viewport_height;

		w = normalize(lookfrom-lookat);
		u = normalize(cross(vup, w));
		v = cross(w, u);

		origin = lookfrom;
		horizontal = focus_dist * viewport_width * u;
		vertical = focus_dist * viewport_height * v;
		lower_left_corner = origin - horizontal/2 - vertical/2 - focus_dist*w;

		lens_radius = aperture / 2;
		time0 = time0_;
		time1 = time1_;
	}

	ray get_ray(float s, float t) const
	{
		vec3 offset;
		if (lens_radius > 0.001f)
		{
			const vec3 rd = lens_radius * random_in_unit_disk();
			offset = u * rd.x + v * rd.y;
		}
		return ray(
			origin,
			lower_left_corner + s*horizontal + t*vertical - origin - offset,
			random_float(time0, time1)
		);
	}

private:
	point3 origin;
	point3 lower_left_corner;
	vec3 horizontal;
	vec3 vertical;
	vec3 u, v, w;
	float lens_radius;
	float time0, time1; // shutter open/close times
};