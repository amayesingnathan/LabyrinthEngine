#pragma once

#include "Labyrinth/Core/Layer.h"
#include "Labyrinth/Events/KeyEvent.h"
#include "Labyrinth/Events/MouseEvent.h"
#include "Labyrinth/IO/Filesystem.h"
#include "Labyrinth/Renderer/EditorCamera.h"
#include "Labyrinth/Renderer/Framebuffer.h"
#include "Labyrinth/Renderer/Texture.h"
#include "Labyrinth/Scene/Entity.h"
#include "Labyrinth/Scene/Scene.h"

#include <glm/glm.hpp>

#include <filesystem>

namespace Labyrinth {

	struct EditorData
	{
		std::string currentFile;
		bool displayColliders = false;
		glm::vec4 colliderColour = { 0, 1, 0, 1 };
		glm::vec4 selectionColour = { 1, 0.4f, 0, 1 };
		bool linkOnDestroy = false;

		EditorCamera camera;
		std::string projectName;
		fs::path projectFilepath;
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

		void LoadSettings();
		void WriteSettings();

		void CreateProject(const fs::path& filepath);
		void OpenProject();
		void OpenProject(const fs::path& filepath);
		void SaveProject();
		void CloseProject(bool unloadProject = true);

		void NewScene();
		bool OpenScene();
		bool OpenScene(const fs::path& path);
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
		void UI_Toolbar();

		static void RegenScriptProject(const fs::path& filepath);

	private:
		Ref<Framebuffer> mFramebuffer;

		Ref<Scene> mCurrentScene, mEditorScene;

		EditorData mEditorData;
		Entity mHoveredEntity;

		Ref<class ScenePanel> mScenePanel = nullptr;

		bool mViewportFocused = false, mViewportHovered = false;
		glm::vec2 mViewportSize = { 0.0f, 0.0f };
		std::array<glm::vec2, 2> mViewportBounds = {};

		i32 mGizmoType = -1;

		enum class SceneState
		{
			Edit = 0, Play = 1, Simulate = 2
		};
		SceneState mSceneState = SceneState::Edit;

		friend class OptionsPanel;
	};

}