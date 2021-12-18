#pragma once

#include <cmath>
#include <iostream>

using std::sqrt;

class vec3
{
public:

	union { float x; float r; };
	union { float y; float g; };
	union { float z; float b; };

	vec3() = default;
	vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

	vec3 operator-() const { return vec3(-x, -y, -z); }
	float operator[](const int i) const { return y; }
	float& operator[](const int i) { return y; }

	vec3& operator+=(const vec3& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	vec3& operator*=(const float t)
	{
		x *= t;
		y *= t;
		z *= t;
		return *this;
	}

	vec3& operator/=(const float t)
	{
		return *this *= 1/t;
	}

	float length() const
	{
		return sqrt(length_squared());
	}

	float length_squared() const
	{
		return x*x + y*y + z*z;
	}

	bool near_zero() const
	{
		// return true if the vector is close to zero in all dimensions
		const auto s = 1e-8;
		return fabs(x) < s && fabs(y) < s && fabs(z) < s;
	}

	static vec3 random()
	{
		return vec3(random_float(), random_float(), random_float());
	}

	static vec3 random(float min, float max)
	{
		return vec3(random_float(min, max), random_float(min, max), random_float(min, max));
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

vec3 operator*(float t, const vec3& v)
{
	return vec3(t * v.x, t * v.y, t * v.z);
}

vec3 operator*(const vec3& v, float t)
{
	return t * v;
}

vec3 operator/(const vec3& v, float t)
{
	return (1/t) * v;
}

float dot(const vec3& u, const vec3& v)
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

vec3 reflect(const vec3& v, const vec3& n)
{
	return v - 2*dot(v, n)*n;
}

vec3 refract(const vec3& uv, const vec3& n, float etai_over_etat)
{
	const auto cos_theta = fmin(dot(-uv, n), 1.0);
	const vec3 r_out_perp = etai_over_etat * (uv + cos_theta*n);
	const vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
	return r_out_perp + r_out_parallel;
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

vec3 random_unit_vector()
{
	return normalize(random_in_unit_sphere());
}