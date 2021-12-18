#pragma once

#include "rtweekend.h"
#include "hittable.h"

class moving_sphere : public hittable
{
public:
	moving_sphere() {}
	moving_sphere(
		point3 cen0, point3 cen1, float time0_, float time1_, float r, shared_ptr<material> m)
		: center0(cen0), center1(cen1), time0(time0_), time1(time1_), radius(r), mat_ptr(m)
	{}

	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override;

	point3 get_center(double time) const;

	point3 center0, center1;
	float time0, time1;
	float radius;
	shared_ptr<material> mat_ptr;
};

point3 moving_sphere::get_center(double time) const
{
	return center0 + ((time - time0) / (time1-time0))*(center1-center0);
}

bool moving_sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
{
	const point3 center = get_center(r.time);
	const vec3 oc = r.origin - center;
	const auto a = r.dir.length_squared();
	const auto half_b = dot(oc, r.dir);
	const auto c = oc.length_squared() - radius * radius;

	const auto discriminant = half_b * half_b - a * c;
	if (discriminant < 0) return false;
	const auto sqrtd = sqrt(discriminant);

	// Find the nearest root that lies in the acceptable range
	auto root = (-half_b - sqrtd) / a;
	if (root < t_min || t_max < root)
	{
		root = (-half_b + sqrtd) / a;
		if (root < t_min || t_max < root)
		{
			return false;
		}
	}

	rec.t = root;
	rec.p = r.at(rec.t);
	const vec3 outward_normal = (rec.p - center) / radius;
	rec.set_face_normal(r, outward_normal);
	rec.mat_ptr = mat_ptr;
	return true;
}