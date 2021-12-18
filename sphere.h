#pragma once

#include "hittable.h"
#include "vec3.h"

class sphere : public hittable
{
public:
	sphere() {}
	sphere(const point3& cen, float r, const shared_ptr<material> m)
		: center(cen), radius(r), mat_ptr(m) {};

	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override;

	point3 center;
	float radius;
	shared_ptr<material> mat_ptr;
};

bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
{
	const vec3 oc = r.origin - center;
	const auto a = r.dir.length_squared();
	const auto half_b = dot(oc, r.dir);
	const auto c = oc.length_squared() - radius*radius;

	const auto discriminant = half_b*half_b - a*c;
	if (discriminant < 0) return false;
	const auto sqrtd = sqrt(discriminant);

	// Find the nearest root that lies in the acceptable range
	auto root = (-half_b - sqrtd) / a;
	if (root < t_min || t_max < root)
	{
		root = (-half_b + sqrtd) / a;
		if(root < t_min || t_max < root)
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