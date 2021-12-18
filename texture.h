#pragma once

#include "rtweekend.h"

class texture
{
public:
	virtual color value(float u, float v, const point3& p) const = 0;
};

class solid_color : public texture
{
public:
	solid_color() {}
	solid_color(color c) : color_value(c) {}
	solid_color(float red, float green, float blue) : color_value(red, green, blue) {}

	virtual color value(float u, float v, const point3& p) const override { return color_value; }

	color color_value;
};

class checker_texture : public texture
{
public:
	checker_texture() {}
	checker_texture(const shared_ptr<texture>& _even, const shared_ptr<texture>& _odd) : even(_even), odd(_odd) {}
	checker_texture(const color& c1, const color& c2) : even(make_shared<solid_color>(c1)), odd(make_shared<solid_color>(c2)) {}

	virtual color value(float u, float v, const point3& p) const override
	{
		const auto sines = sin(10*p.x) * sin(10*p.y) * sin(10*p.z);
		const auto& selected = sines < 0 ? odd : even;
		return selected->value(u, v, p);
	}

	shared_ptr<texture> odd, even;
};