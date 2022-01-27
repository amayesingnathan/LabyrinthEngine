#pragma once

#include "Labyrinth.h"
#include "Panels/ScenePanel.h"
#include "Panels/ContentBrowserPanel.h"

#include "Labyrinth/Renderer/EditorCamera.h"

#include <optional>

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
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveScene();
		void SaveSceneAs();

		void OnScenePlay();
		void OnSceneStop();

		// UI Panels
		void UI_Toolbar();

	private:
		Ref<Framebuffer> mFramebuffer;

		ScenePanel mScenePanel;
		Ref<Scene> mCurrentScene;

		ContentBrowserPanel mContentBrowserPanel;

		Entity mHoveredEntity;

		std::string mFilepath;

		bool mPrimaryCamera = true;

		EditorCamera mEditorCamera;

		Ref<Texture2D> mCheckerboardTexture;
		
		// Editor resources
		Ref<Texture2D> mIconPlay, mIconStop;

		bool mViewportFocused = false, mViewportHovered = false;
		glm::vec2 mViewportSize = { 0.0f, 0.0f };
		std::array<glm::vec2, 2> mViewportBounds = {};

		glm::vec4 mSquareColour = { 0.2f, 0.3f, 0.8f, 1.0f };
		int mGizmoType = -1;

		enum class SceneState
		{
			Edit = 0, Play = 1
		};
		SceneState mSceneState = SceneState::Edit;
	};

}