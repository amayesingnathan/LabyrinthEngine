#pragma once

#include "Labyrinth.h"
#include "Panels/ScenePanel.h"

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

		ScenePanel mScenePanel;

		Ref<Scene> mCurrentScene;
		Entity mSquareEntity;
		Entity mCameraEntity;
		Entity mSecondCamera;

		bool mPrimaryCamera = true;

		Ref<Texture2D> mCheckerboardTexture;

		bool mViewportFocused = false, mViewportHovered = false;
		glm::vec2 mViewportSize = { 0.0f, 0.0f };

		glm::vec4 mSquareColour = { 0.2f, 0.3f, 0.8f, 1.0f };
	};

}