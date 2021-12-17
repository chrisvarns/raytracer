#pragma once

#include <cmath>
#include <iostream>

using std::sqrt;

class vec3
{
public:

	union { double x; double r; };
	union { double y; double g; };
	union { double z; double b; };

	vec3() = default;
	vec3(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}

	vec3 operator-() const { return vec3(-x, -y, -z); }
	double operator[](const int i) const { return y; }
	double& operator[](const int i) { return y; }

	vec3& operator+=(const vec3& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	vec3& operator*=(const double t)
	{
		x *= t;
		y *= t;
		z *= t;
		return *this;
	}

	vec3& operator/=(const double t)
	{
		return *this *= 1/t;
	}

	double length() const
	{
		return sqrt(length_squared());
	}

	double length_squared() const
	{
		return x*x + y*y + z*z;
	}

	static vec3 random()
	{
		return vec3(random_double(), random_double(), random_double());
	}

	static vec3 random(double min, double max)
	{
		return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
	}
};

// Type aliases for vec3
using point3 = vec3;	// 3D point
using color = vec3;		// RGB color

vec3 operator+(const vec3& u, const vec3& v)
{
	return vec3(u.x + v.x, u.y + v.y, u.z + v.z);
}

vec3 operator-(const vec3& u, const vec3& v)
{
	return vec3(u.x - v.x, u.y - v.y, u.z - v.z);
}

vec3 operator*(const vec3& u, const vec3& v)
{
	return vec3(u.x * v.x, u.y * v.y, u.z * v.z);
}

vec3 operator*(double t, const vec3& v)
{
	return vec3(t * v.x, t * v.y, t * v.z);
}

vec3 operator*(const vec3& v, double t)
{
	return t * v;
}

vec3 operator/(const vec3& v, double t)
{
	return (1/t) * v;
}

double dot(const vec3& u, const vec3& v)
{
	return u.x * v.x
		 + u.y * v.y
		 + u.z * v.z;
}

vec3 cross(const vec3& u, const vec3& v)
{
	return vec3(u.y * v.z - u.z * v.y,
				u.z * v.x - u.x * v.z,
				u.x * v.y - u.y * v.x);
}

vec3 normalize(const vec3& v)
{
	return v/v.length();
}

vec3 random_in_unit_sphere()
{
	while (true)
	{
		const auto p = vec3::random(-1, 1);
		if (p.length_squared() >= 1) continue;
		return p;
	}
}