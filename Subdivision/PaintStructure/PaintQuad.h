#pragma once

struct PaintQuad
{
	unsigned int v[4];

	PaintQuad(unsigned int x = 0, unsigned int y = 0, unsigned int z = 0, unsigned int w = 0)
	{
		v[0] = x;
		v[1] = y;
		v[2] = z;
		v[3] = w;
	}
};