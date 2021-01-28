#pragma once

struct PaintTri
{
	unsigned int v[3];

	PaintTri(unsigned int v0 = 0, unsigned int v1 = 0, unsigned int v2 = 0)
	{
		v[0] = v0;	v[1] = v1; v[2] = v2;
	}
};