#pragma once

namespace Labyrinth {

	struct CellComponent
	{
		unsigned int x;
		unsigned int y;

		CellComponent(unsigned int _x, unsigned int _y)
			: x(_x), y(_y) {}
	};

	struct FluidComponent
	{
		unsigned int size;
		float diff;
		float visc;

		float* s;
		float* density;

		float* vX;
		float* vY;

		float* vX0;
		float* vY0;
	};

}