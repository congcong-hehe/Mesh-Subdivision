#pragma once

#include "REAL.h"
#include "HalfEdge.h"

struct Vec3f
{
	REAL x, y, z;
	HalfEdge* halfEdge;

	Vec3f(REAL _x = 0, REAL _y = 0, REAL _z = 0, HalfEdge* _halfEdge = nullptr) : x(_x), y(_y), z(_z), halfEdge(_halfEdge) {}

	Vec3f operator + (const Vec3f& v)
	{
		return Vec3f(x + v.x, y + v.y, z + v.z);
	}

	Vec3f operator - (const Vec3f& v)
	{
		return Vec3f(x - v.x, y - v.y, z - v.z);
	}

	Vec3f operator * (const float a)
	{
		return Vec3f(x * a, y * a, z * a);
	}

	Vec3f operator / (const float a)
	{
		return Vec3f(x / a, y / a, z / a);
	}

	Vec3f operator += (const Vec3f& v)
	{
		x += v.x; y += v.y; z += v.z;
		return *this;
	}
};