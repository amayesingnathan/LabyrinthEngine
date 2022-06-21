#pragma once

#include "Labyrinth.h"
#include "Panels/ScenePanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Panels/SpriteSheetPanel.h"

#include "Labyrinth/Renderer/EditorCamera.h"

namespace Labyrinth {

	struct EditorData
	{
		std::string currentFile;
		bool displayColliders = false;
		glm::vec4 colliderColour = { 0, 1, 0, 1 };
		bool linkOnDestroy = false;
	};

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
		bool OpenScene();
		bool OpenScene(const std::filesystem::path& path);
		void SaveScene();
		void SaveSceneAs();

		void OnScenePlay();
		void OnSceneSimulate();
		void OnSceneStop();

		void OnOverlayRender();

		void SetCurrentScene(const Ref<Scene>& currentScene);
		void SyncWindowTitle();

		void CloneEntity();

		// UI Panels
		void UI_Viewport();
		void UI_Gizmos();
		void UI_MenuBar();
		void UI_ChildPanels();
		void UI_Stats();
		void UI_Options();
		void UI_Toolbar();

	private:
		Ref<Framebuffer> mFramebuffer;

		Ref<Scene> mCurrentScene, mEditorScene;

		ScenePanel mScenePanel;
		ContentBrowserPanel mContentBrowserPanel;
		SpriteSheetPanel mSpriteSheetPanel;

		EditorData mEditorData;
		EditorCamera mEditorCamera;

		Entity mHoveredEntity;
		
		// Editor resources
		Ref<Texture2D> mIconPlay, mIconStop, mIconSim;
		Ref<Texture2D> mHighlight;

		bool mViewportFocused = false, mViewportHovered = false;
		glm::vec2 mViewportSize = { 0.0f, 0.0f };
		std::array<glm::vec2, 2> mViewportBounds = {};

		int mGizmoType = -1;

		enum class SceneState
		{
			Edit = 0, Play = 1, Simulate = 2
		};
		SceneState mSceneState = SceneState::Edit;
	};

}