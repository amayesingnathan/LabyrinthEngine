#pragma once

#include "Labyrinth.h"

namespace Labyrinth {

	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void onAttach() override;
		virtual void onDetach() override;

		virtual void onUpdate(Timestep ts) override;
		virtual void onImGuiRender() override;
		virtual void onEvent(Event& e);

	private:
		Labyrinth::OrthographicCameraController mCameraController;

		//Temp
		Ref<VertexArray> mSquareVA;
		Ref<Shader> mFlatColourShader;
		Ref<Framebuffer> mFramebuffer;

		Ref<Texture2D> mCheckerboardTexture;

		glm::vec4 mSquareColour = { 0.2f, 0.3f, 0.8f, 1.0f };
	};

}