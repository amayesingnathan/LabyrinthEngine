#pragma once

#include "Labyrinth.h"
#include "Fluid.h"

namespace Labyrinth {

	class FluidLayer : public Layer
	{
	public:
		FluidLayer();
		virtual ~FluidLayer() = default;

		virtual void onAttach() override;
		virtual void onDetach() override;

		void onUpdate(Timestep ts) override;
		virtual void onImGuiRender() override;
		void onEvent(Event& e) override;

	private:
		Ref<Scene> mScene;
		Ref<Fluid> mFluid;

		Ref<Framebuffer> mFramebuffer;

		bool mViewportHovered = false;
		glm::vec2 mViewportSize = { 0.0f, 0.0f };
		std::array<glm::vec2, 2> mViewportBounds = {};
		glm::vec2 mPrevMousePos = { 0.f, 0.f };
	};

}
