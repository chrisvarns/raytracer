#pragma once

#include "rtweekend.h"

struct hit_record;

class material
{
public:
	virtual color get_albedo() const = 0;
	virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const = 0;
};

class lambertian : public material
{
public:
	lambertian(const color& a) : albedo(a) {}

	virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
	{
		vec3 scatter_direction = rec.normal + random_unit_vector();

		// catch degenerate scatter direction
		if(scatter_direction.near_zero())
			scatter_direction = rec.normal;

		scattered = ray(rec.p, scatter_direction, r_in.time);
		attenuation = albedo;
		return true;
	}

	virtual color get_albedo() const override { return albedo; }

	color albedo;
};

class metal : public material
{
public:
	metal(const color& a, float f) : albedo(a), fuzz(f < 1 ? f : 1) {}

	virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
	{
		const vec3 reflected = reflect(normalize(r_in.dir), rec.normal);
		scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere(), r_in.time);
		attenuation = albedo;
		return dot(scattered.dir, rec.normal) > 0;
	}

	virtual color get_albedo() const override { return albedo; }

	color albedo;
	float fuzz;
};

class dielectric : public material
{
public:
	dielectric(float ir_) : ir(ir_) {}

	virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
	{
		attenuation = color(1.0, 1.0, 1.0);
		const float refraction_ratio = rec.front_face ? (1.0/ir) : ir;

		const vec3 unit_direction = normalize(r_in.dir);
		const float cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
		const float sin_theta = sqrt(1.0 - cos_theta*cos_theta);

		const bool cannot_refract = refraction_ratio * sin_theta > 1.0;
		const vec3 direction = cannot_refract || reflectance(cos_theta, refraction_ratio) > random_float()
			? reflect(unit_direction, rec.normal)
			: refract(unit_direction, rec.normal, refraction_ratio);

		scattered = ray(rec.p, direction, r_in.time);
		return true;
	}

	virtual color get_albedo() const override { return color(1,1,1); }

	float ir; // index of refraction

private:
	static float reflectance(float cosine, float ref_idx)
	{
		// use schlick's approximation for reflectance
		auto r0 = (1.0-ref_idx) / (1.0+ref_idx);
		r0 = r0*r0;
		return r0 + (1.0-r0)*pow(1.0-cosine, 5.0);
	}
};