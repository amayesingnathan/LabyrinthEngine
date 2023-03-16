#pragma once

#include "Labyrinth.h"

namespace Laby {

	class SandboxLayer : public Layer
	{
	public:
		SandboxLayer() = default;

		virtual void onAttach() override;
		virtual void onDetach() override;

		void onUpdate(Timestep ts) override;
		virtual void onImGuiRender() override;
		void onEvent(Event& e) override;
		LISTENING_EVENTS(KeyPressed)

	private:
		bool OnKeyPressedEvent(KeyPressedEvent& e);

	private:
		Ref<Framebuffer> mFramebuffer;
		Ref<Texture2D> mCheckerboardTexture;

		Ref<Texture2DSheet> mSpriteSheet;
		Ref<SubTexture2D> mTNT;

		Ref<OrthographicCamera> mCamera = nullptr;

		glm::vec4 mSquareColours[4];
	};
}