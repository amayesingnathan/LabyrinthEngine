#include "FluidLayer.h"

#include "FluidComponents.h"

namespace Labyrinth {

	FluidLayer::FluidLayer()
		: Layer("FluidLayer")
	{
	}

	void FluidLayer::onAttach()
	{
		LAB_PROFILE_FUNCTION();

		mScene = CreateRef<Scene>();

		FluidSpec spec;
		spec.diff = 1.f; spec.visc = 1.f; spec.size = 100;
		mFluid = CreateRef<Fluid>(mScene, spec);

		for (uint i = 0; i < spec.size; i++) {
			for (uint j = 0; j < spec.size; j++) {
				Entity cellEnt = mScene->CreateEntity("");
				auto& trans = cellEnt.getComponent<TransformComponent>().translation;
				trans.x = Cast<float>(i);
				trans.y = Cast<float>(j);

				cellEnt.addComponent<CellComponent>(i, j);
				cellEnt.addComponent<SpriteRendererComponent>(glm::vec4{ 0.f, 0.f, 0.f, 1.f });
			}
		}
	}

	void FluidLayer::onDetach()
	{
		LAB_PROFILE_FUNCTION();
	}

	void FluidLayer::onUpdate(Timestep ts)
	{
		LAB_PROFILE_FUNCTION();

		if (Input::IsMouseButtonPressed(Mouse::ButtonLeft))
		{
			glm::vec2 mousePos = Input::GetMousePosition();
			glm::vec2 cellSize = Application::Get().getWindow().getSize();
			uint meshSize = mFluid->getSize();
			uint x = Cast<uint>(floorf((mousePos.x / cellSize.x) * meshSize));
			uint y = Cast<uint>(floorf((mousePos.y / cellSize.y) * meshSize));
			mFluid->addDensity(x, y, 1.f);
		}

		mFluid->onUpdate(ts);
	}

	void FluidLayer::onImGuiRender()
	{
		LAB_PROFILE_FUNCTION();
	}

	void FluidLayer::onEvent(Event& e)
	{
		LAB_PROFILE_FUNCTION();
	}

	template<>
	void Scene::onComponentAdded<CellComponent>(Entity entity, CellComponent& component)
	{
	}
	template<>
	void Scene::onComponentAdded<FluidComponent>(Entity entity, FluidComponent& component)
	{
	}
}