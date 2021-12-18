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