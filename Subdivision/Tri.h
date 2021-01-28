#pragma once

#include "HalfEdge.h"

struct Tri
{
	unsigned int v[3];
	HalfEdge* halfedge;

	Tri(unsigned int x = 0, unsigned int y = 0,unsigned int z = 0, HalfEdge* _halfedge = nullptr)
	{
		v[0] = x;
		v[1] = y;
		v[2] = z;
		halfedge = _halfedge;
	}
};