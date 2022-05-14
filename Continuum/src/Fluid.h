#pragma once

#include "FluidComponents.h"

#include <Labyrinth/Scene/Entity.h>

#define ITERATIONS 20

using uint = unsigned int;

namespace Labyrinth {

	class Timestep;

	struct FluidSpec
	{
		unsigned int size;
		float diff;
		float visc;
	};

	class Fluid
	{
	private:
		Entity mEntity;
		Ref<Scene> mCurrentScene;
		uint mSize;

	public:
		Fluid(Ref<Scene> scene, const FluidSpec& spec)
			: mCurrentScene(scene)
		{
			mEntity = mCurrentScene->CreateEntity("Fluid");
			auto& fluid = mEntity.addComponent<FluidComponent>();
			mSize = spec.size;
			fluid.size = mSize;
			fluid.diff = spec.diff;
			fluid.visc = spec.visc;

			fluid.s = new float[mSize * mSize]{ 0.f };
			fluid.density = new float[mSize * mSize]{ 0.f };

			fluid.vX = new float[mSize * mSize]{0.f};
			fluid.vY = new float[mSize * mSize]{ 0.f };

			fluid.vX0 = new float[mSize * mSize]{ 0.f };
			fluid.vY0 = new float[mSize * mSize]{ 0.f };
		}

		uint getSize() const { return mEntity.getComponent<FluidComponent>().size; }

		void onUpdate(Timestep ts);

		void addDensity(uint x, uint y, float amount);
		void addVelocity(uint x, uint y, float Vx, float Vy);

	private:
		void Diffuse(int b, float* x, float* x0, float diff, float dt);
		void Project(float* vX, float* vY, float* p, float* div);
		void Advect(int b, float* d, float* d0, float* vX, float* vY, float dt);

		void LinSolve(int b, float* x, float* x0, float a, float c);
		void SetBound(int b, float* x);

		void UpdateGrid();

		inline uint IX(uint x, uint y) { return (x + y * mSize); }
	};
}