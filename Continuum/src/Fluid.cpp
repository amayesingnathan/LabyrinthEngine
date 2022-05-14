#include "Fluid.h"

#include "Labyrinth.h"

namespace Labyrinth {

	static const glm::vec4 Black = { 0.f, 0.f, 0.f, 1.f };

	static std::map<float, glm::vec4> ColourMap =
	{
		{0.0001f,	{ 0.321f, 0.f, 0.066f, 1.0f } },
		{0.0800f,	{ 0.454f, 0.f, 0.027f, 1.0f } },
		{0.1600f,	{ 0.592f, 0.023f, 0.f, 1.0f } },
		{0.2400f,	{ 0.729f, 0.145f, 0.f, 1.0f } },
		{0.3200f,	{ 0.862f, 0.301f, 0.f, 1.0f } },
		{0.4000f,	{ 0.996f, 0.498f, 0.f, 1.0f } },
		{0.4800f,	{ 1.f, 0.513f, 0.090f, 1.0f } },
		{0.5600f,	{ 1.f, 0.533f, 0.184f, 1.0f } },
		{0.6400f,	{ 1.f, 0.556f, 0.274f, 1.0f } },
		{0.7200f,	{ 1.f, 0.584f, 0.368f, 1.0f } },
		{0.8000f,	{ 1.f, 0.619f, 0.458f, 1.0f } },
		{0.8800f,	{ 1.f, 0.662f, 0.549f, 1.0f } }
	};

	static void DensityToColour(const float density, glm::vec4& colourOut)
	{
		if (density == 0.f) { colourOut = Black; return; }
		auto it = std::find_if(ColourMap.rbegin(), ColourMap.rend(), [density](const auto& colMapEntry) {
			return density > colMapEntry.first;
		});
		if (it == ColourMap.rend()) { colourOut = Black; return; }

		colourOut = it->second;
	}

	void Fluid::onUpdate(Timestep ts)
	{
		auto& fluid = mEntity.getComponent<FluidComponent>();
		float visc = fluid.visc;
		float diff = fluid.diff;
		float* Vx = fluid.vX;
		float* Vy = fluid.vY;
		float* Vx0 = fluid.vX0;
		float* Vy0 = fluid.vY0;
		float* s = fluid.s;
		float* dens = fluid.density;

		Diffuse(1, Vx0, Vx, visc, ts);
		Diffuse(2, Vy0, Vy, visc, ts);

		Project(Vx0, Vy0, Vx, Vy);

		Advect(1, Vx, Vx0, Vx0, Vy0, ts);
		Advect(2, Vy, Vy0, Vx0, Vy0, ts);

		Project(Vx, Vy, Vx0, Vy0);

		Diffuse(0, s, dens, diff, ts);
		Advect(0, dens, s, Vx, Vy, ts);

		UpdateGrid();
	}

	void Fluid::addDensity(uint x, uint y, float amount)
	{
		auto& fluid = mEntity.getComponent<FluidComponent>();
		fluid.density[IX(x, y)] += amount;
	}

	void Fluid::addVelocity(uint x, uint y, float Vx, float Vy)
	{
		auto& fluid = mEntity.getComponent<FluidComponent>();
		uint index = IX(x, y);
		fluid.vX[index] += Vx;
		fluid.vY[index] += Vy;
	}

	void Fluid::Diffuse(int b, float* x, float* x0, float diff, float dt)
	{
		float a = dt * diff * (mSize - 2) * (mSize - 2);
		LinSolve(b, x, x0, a, 1.f + 6.f * a);
	}

	void Fluid::Project(float* vX, float* vY, float* p, float* div)
	{
		for (uint j = 1; j < mSize - 1; j++) {
			for (uint i = 1; i < mSize - 1; i++) {
				div[IX(i, j)] = -0.5f * (
					vX[IX(i + 1, j)] - vX[IX(i - 1, j)] +
					vX[IX(i, j + 1)] - vX[IX(i, j - 1)]
					) / mSize;
				p[IX(i, j)] = 0;
			}
		}

		SetBound(0, div);
		SetBound(0, p);
		LinSolve(0, p, div, 1, 6);

		for (uint j = 1; j < mSize - 1; j++) {
			for (uint i = 1; i < mSize - 1; i++) {
				vX[IX(i, j)] -= 0.5f * (p[IX(i + 1, j)] - p[IX(i - 1, j)]) * mSize;
				vY[IX(i, j)] -= 0.5f * (p[IX(i, j + 1)] - p[IX(i, j - 1)]) * mSize;
			}
		}

		SetBound(1, vX);
		SetBound(2, vY);
	}

	void Fluid::Advect(int b, float* d, float* d0, float* vX, float* vY, float dt)
	{
		float i0, i1, j0, j1;

		float dtx = dt * (mSize - 2);
		float dty = dt * (mSize - 2);

		float s0, s1, t0, t1;
		float tmp1, tmp2, x, y;

		float sizeF = (float)mSize;
		float iF, jF;
		uint i, j;

		for (j = 1, jF = 1.f; j < mSize - 1; j++, jF++) {
			for (i = 1, iF = 1.f; i < mSize - 1; i++, iF++) {
				tmp1 = dtx * vX[IX(i, j)];
				tmp2 = dty * vY[IX(i, j)];
				x = iF - tmp1;
				y = jF - tmp2;

				if (x < 0.5f) x = 0.5f;
				if (x > sizeF + 0.5f) x = sizeF + 0.5f;
				i0 = floorf(x);
				i1 = i0 + 1.f;

				if (y < 0.5f) y = 0.5f;
				if (y > sizeF + 0.5f) y = sizeF + 0.5f;
				j0 = floorf(y);
				j1 = j0 + 1.f;

				s1 = x - i0;
				s0 = 1.f - s1;	
				t1 = y - j0;
				t0 = 1.f - t1;

				uint i0i = Cast<uint>(i0);
				uint i1i = Cast<uint>(i1);
				uint j0i = Cast<uint>(j0);
				uint j1i = Cast<uint>(j1);

				d[IX(i, j)] =	s0 * (t0 * d0[IX(i0i, j0i)] + t1 * d0[IX(i0i, j1i)]) +
								s1 * (t0 * d0[IX(i1i, j0i)] + t1 * d0[IX(i1i, j1i)]);
			}
		}

		SetBound(b, d);
	}

	void Fluid::LinSolve(int b, float* x, float* x0, float a, float c)
	{
		float cRecip = 1.f / c;
		for (uint k = 0; k < ITERATIONS; k++) {
			for (uint j = 1; j < mSize - 1; j++) {
				for (uint i = 1; i < mSize - 1; i++) {
					x[IX(i, j)] = 
						(x0[IX(i, j)] + a * (
							x[IX(i + 1, j)] + 
							x[IX(i - 1, j)] +
							x[IX(i, j + 1)] + 
							x[IX(i, j - 1)]
						)) * cRecip;
				}
			}
		}
	}

	void Fluid::SetBound(int b, float* x)
	{
		for (uint i = 1; i < mSize - 1; i++) {
			x[IX(i, 0)] = (b == 2) ? -x[IX(i, 1)] : x[IX(i, 1)];
			x[IX(i, mSize - 1)] = (b == 2) ? -x[IX(i, mSize - 2)] : x[IX(i, mSize - 2)];
		}

		for (uint j = 1; j < mSize - 1; j++) {
			x[IX(0, j)] = (b == 2) ? -x[IX(1, j)] : x[IX(1, j)];
			x[IX(mSize - 1, j)] = (b == 2) ? -x[IX(mSize - 2, j)] : x[IX(mSize - 2, j)];
		}

		x[IX(0, 0)] = 0.5f * (x[IX(1, 0)] + x[IX(0, 1)]);
		x[IX(0, mSize - 1)] = 0.5f * (x[IX(1, mSize - 1)] + x[IX(0, mSize - 2)]);
		x[IX(mSize - 1, 0)] = 0.5f * (x[IX(mSize - 2, 0)] + x[IX(mSize - 1, 1)]);
		x[IX(mSize - 1, mSize - 1)] = 0.5f * (x[IX(mSize - 2, mSize - 1)] + x[IX(mSize - 1, mSize - 2)]);
	}


	void Fluid::UpdateGrid()
	{
		auto& fluid = mEntity.getComponent<FluidComponent>();
		mCurrentScene->view<CellComponent, SpriteRendererComponent>().each([this, &fluid](const CellComponent& cell, SpriteRendererComponent& tile) {
			float cellDens = fluid.s[IX(cell.x, cell.y)];
			DensityToColour(cellDens, tile.colour);
		});

	}
}