#pragma once
#include "HalfEdge.h"

struct Quad
{
	unsigned int v[4];
	HalfEdge* halfedge;

	Quad(unsigned int x = 0, unsigned int y = 0, unsigned int z = 0, unsigned int w = 0, HalfEdge* halfedge = nullptr)
	{
		v[0] = x;
		v[1] = y;
		v[2] = z;
		v[3] = w;

		this->halfedge = halfedge;
	}
};
