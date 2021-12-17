#pragma once

#include "rtweekend.h"

struct hit_record;

class material
{
public:
	virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const = 0;
};

class lambertian : public material
{
public:
	lambertian(const color& a) : albedo(a) {}

	virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
	{
		auto scatter_direction = rec.normal + random_unit_vector();

		// catch degenerate scatter direction
		if(scatter_direction.near_zero())
			scatter_direction = rec.normal;

		scattered = ray(rec.p, scatter_direction);
		attenuation = albedo;
		return true;
	}

	color albedo;
};

class metal : public material
{
public:
	metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

	virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
	{
		const vec3 reflected = reflect(normalize(r_in.dir), rec.normal);
		scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere());
		attenuation = albedo;
		return dot(scattered.dir, rec.normal) > 0;
	}

	color albedo;
	double fuzz;
};

class dielectric : public material
{
public:
	dielectric(double ir_) : ir(ir_) {}

	virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
	{
		attenuation = color(1.0, 1.0, 1.0);
		const double refraction_ratio = rec.front_face ? (1.0/ir) : ir;

		const vec3 unit_direction = normalize(r_in.dir);
		const double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
		const double sin_theta = sqrt(1.0 - cos_theta*cos_theta);

		const bool cannot_refract = refraction_ratio * sin_theta > 1.0;
		const vec3 direction = cannot_refract ? reflect(unit_direction, rec.normal) : refract(unit_direction, rec.normal, refraction_ratio);

		scattered = ray(rec.p, direction);
		return true;
	}

	double ir; // index of refraction
};