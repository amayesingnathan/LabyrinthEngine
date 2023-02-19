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

		OrthographicCamera mCamera = OrthographicCamera(0, 1, 0, 1);

		glm::vec4 mSquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
		glm::vec4 mSquareColor2 = { 0.8f, 0.5f, 0.1f, 1.0f };
	};
}