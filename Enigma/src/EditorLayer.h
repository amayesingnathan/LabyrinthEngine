#pragma once

#include "Labyrinth/Core/Layer.h"
#include "Labyrinth/Editor/EditorData.h"
#include <Labyrinth/Editor/Panels/ScenePanel.h>
#include <Labyrinth/Editor/Panels/EntityPanel.h>
#include "Labyrinth/Renderer/Framebuffer.h"
#include "Labyrinth/Scene/Scene.h"

namespace Laby {

	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void onAttach() override;
		virtual void onDetach() override;

		virtual void onUpdate(Timestep ts) override;
		virtual void onImGuiRender() override;

		virtual void onEvent(Event& e) override;
		LISTENING_EVENTS(KeyPressed, MouseButtonPressed)

	private:
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

	private:
		void OnOverlayRender();

		// UI Panels
		void UI_Viewport();
		void UI_Gizmos();
		void UI_MenuBar();
		void UI_Toolbar();

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

		void DrawScene(Ref<Scene> scene, Timestep ts);

		void OnSceneChange(Ref<Scene> scene);
		void SyncWindowTitle(Ref<Scene> scene = nullptr);

		void CloneEntity();

	private:
		Ref<Framebuffer> mFramebuffer;

		Ref<SceneManager> mSceneManager;
		UUID mEditorScene = 0, mRuntimeScene = 0;

		EditorData mEditorData;

		Ref<ScenePanel> mScenePanel;
		Ref<EntityPanel> mEntityPanel;

		enum SceneState { SceneEdit = 0, ScenePlay, SceneSimulate };
		SceneState mSceneState = SceneEdit;

		friend class OptionsPanel;
	};
}