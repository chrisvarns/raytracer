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
	virtual bool bounding_box(float time0, float time1, aabb& output_box) const override;

	point3 center;
	float radius;
	shared_ptr<material> mat_ptr;

private:
	static void get_sphere_uv(const point3& p, float& u, float& v)
	{
		// p: a given point on the sphere of radius one, centered at the origin.
		// u: returned value [0,1] of angle around the Y axis from X=-1.
		// v: returned value [0,1] of angle from Y=-1 to Y=+1.
		//     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
		//     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
		//     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

		const auto theta = acos(-p.y);
		const auto phi = atan2(-p.z, p.x) + pi;

		u = phi / (2*pi);
		v = theta / pi;
	}
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
	get_sphere_uv(outward_normal, rec.u, rec.v);
	rec.mat_ptr = mat_ptr;
	return true;
}

bool sphere::bounding_box(float time0, float time1, aabb& output_box) const
{
	output_box = aabb(center - vec3(radius), center + vec3(radius));
	return true;
}