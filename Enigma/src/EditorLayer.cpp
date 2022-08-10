#include "EditorLayer.h"

#include <Labyrinth/Editor/ModalManager.h>

#include "Modals/NewProjectModal.h"
#include "Modals/SettingsModal.h"
#include "Modals/ProjectSettingsModal.h"

#include "Panels/ScenePanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Panels/AssetPanel.h"
#include "Panels/SpriteSheetPanel.h"
#include "Panels/OptionsPanel.h"
#include "Panels/StatsPanel.h"

#include "Labyrinth/Assets/AssetManager.h"

#include "Labyrinth/Editor/EditorResources.h"

#include "Labyrinth/IO/Input.h"

#include "Labyrinth/Project/ProjectSerialiser.h"

#include "Labyrinth/Renderer/Renderer2D.h"
#include "Labyrinth/Renderer/RenderCommand.h"

#include "Labyrinth/Scene/SceneSerialiser.h"

#include "Labyrinth/Tools/PlatformUtils.h"

#include "Labyrinth/Maths/Maths.h"

#include <imgui/imgui.h>
#include "ImGuizmo.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cstdlib>

namespace Labyrinth {

	static void ReplaceToken(std::string& str, const char* token, const std::string& value)
	{
		usize pos = 0;
		while ((pos = str.find(token, pos)) != std::string::npos)
		{
			str.replace(pos, strlen(token), value);
			pos += strlen(token);
		}
	}

	EditorLayer::EditorLayer()
		:	Layer("EditorLayer")
	{
	}

	void EditorLayer::onAttach()
	{
		LAB_PROFILE_FUNCTION();

		EditorResources::Init();

		FramebufferSpec fbSpec;
		fbSpec.width = 1600;
		fbSpec.height = 900;
		fbSpec.attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.samples = 1;
			
		mFramebuffer = Framebuffer::Create(fbSpec);

		mViewportSize = { fbSpec.width, fbSpec.height };

		mEditorData.camera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);

		mScenePanel = PanelManager::Register<ScenePanel>("Scene Heirarchy", true, mEditorData);
		PanelManager::Register<ContentBrowserPanel>("Content Browser");
		PanelManager::Register<SpriteSheetPanel>("Sprite Sheets");
		PanelManager::Register<OptionsPanel>("Options", true, mEditorData);
		PanelManager::Register<StatsPanel>("Statistics", true, mHoveredEntity);

		LoadSettings();

		if (!Application::Get().getSpec().startupProject.empty())
			OpenProject(Application::Get().getSpec().startupProject);
		else
			NewScene();
	}

	void EditorLayer::onDetach()
	{
		LAB_PROFILE_FUNCTION();

		EditorResources::Shutdown();

		WriteSettings();
		PanelManager::Clear();

		CloseProject(false);
	}

	void EditorLayer::onUpdate(Timestep ts)
	{
		LAB_PROFILE_FUNCTION();

		if (FramebufferSpec spec = mFramebuffer->getSpecification();
			mViewportSize.x > 0.0f && mViewportSize.y > 0.0f && // zero sized framebuffer is invalid
			(spec.width != mViewportSize.x || spec.height != mViewportSize.y))
		{
			mFramebuffer->resize((u32)mViewportSize.x, (u32)mViewportSize.y);

			mEditorData.camera.setViewportSize(mViewportSize.x, mViewportSize.y);
			mCurrentScene->onViewportResize((u32)mViewportSize.x, (u32)mViewportSize.y);
		}

		Renderer2D::ResetStats();

		PanelManager::UpdatePanels();

		mFramebuffer->bind();
		RenderCommand::SetClearColor({ 0.125f, 0.0625f, 0.25f, 1.0f });
		RenderCommand::Clear();

		// Clear our entity ID attachment to -1
		mFramebuffer->clearAttachment(1, -1);

		switch (mSceneState)
		{
		case SceneState::Edit:
		{
			if (mViewportHovered && mViewportFocused)
				mEditorData.camera.onUpdate(ts);

			mCurrentScene->onUpdateEditor(ts, mEditorData.camera);
			break;
		}
		case SceneState::Play:
		{
			mCurrentScene->onUpdateRuntime(ts);
			break;
		}
		case SceneState::Simulate:
		{
			if (mViewportHovered && mViewportFocused)
				mEditorData.camera.onUpdate(ts);

			mCurrentScene->onUpdateSimulation(ts, mEditorData.camera);
			break;
		}
		}

		auto [mx, my] = ImGui::GetMousePos();
		mx -= mViewportBounds[0].x;	
		my -= mViewportBounds[0].y;
		glm::vec2 viewportSize = mViewportBounds[1] - mViewportBounds[0];
		my = viewportSize.y - my;
		i32 mouseX = (i32)mx;
		i32 mouseY = (i32)my;

		if (mouseX >= 0 && mouseY >= 0 && mouseX < (i32)viewportSize.x && mouseY < (i32)viewportSize.y)
		{
			i32 pixelData = mFramebuffer->readPixel(1, mouseX, mouseY); 
			mHoveredEntity = (pixelData == -1) ? Entity() : Entity((entt::entity)pixelData, mCurrentScene);
		}

		OnOverlayRender();

		mFramebuffer->unbind();
		
	}

	void EditorLayer::onImGuiRender()
	{
		LAB_PROFILE_FUNCTION();

		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.0f;

		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		style.WindowMinSize.x = minWinSizeX;

		UI_Viewport();
		UI_MenuBar();
		UI_ChildPanels();
		UI_Toolbar();

		ModalManager::Display();

		ImGui::End();

	}

	void EditorLayer::UI_Viewport()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");

		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();
		mViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		mViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		mViewportFocused = ImGui::IsWindowFocused();
		mViewportHovered = ImGui::IsWindowHovered();
		Application::Get().getImGuiLayer()->blockEvents(!mViewportFocused && !mViewportHovered);

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		mViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		u32 textureID = mFramebuffer->getColourAttachmentRendererID();
		ImGui::Image((ImTextureID)(intptr_t)textureID, ImVec2{ mViewportSize.x, mViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				fs::path fullPath = Project::GetAssetDirectory() / path;
				if (AssetManager::IsExtensionValid(fullPath.extension().string(), AssetType::Scene))
					OpenScene(fullPath);
			}
			ImGui::EndDragDropTarget();
		}

		if (mSceneState == SceneState::Edit) UI_Gizmos();

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void EditorLayer::UI_Gizmos()
	{
		// Gizmos
		Entity selectedEntity = mScenePanel->getSelectedEntity();
		if (selectedEntity && mGizmoType != -1 && mViewportFocused && selectedEntity.hasComponent<TransformComponent>())
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			float windowWidth = (float)ImGui::GetWindowWidth();
			float windowHeight = (float)ImGui::GetWindowHeight();
			ImGuizmo::SetRect(mViewportBounds[0].x, mViewportBounds[0].y, mViewportBounds[1].x - mViewportBounds[0].x, mViewportBounds[1].y - mViewportBounds[0].y);

			//Editor camera
			const glm::mat4& cameraProjection = mEditorData.camera.getProjection();
			glm::mat4 cameraView = mEditorData.camera.getViewMatrix();

			// Entity transform
			auto& tc = selectedEntity.getComponent<TransformComponent>();
			glm::mat4 transform = tc.getTransform();

			// Snapping
			bool snap = Input::IsKeyPressed(Key::LeftControl);
			float snapValue = 1.0f; // Snap to 0.5m for translation/scale
			// Snap to 45 degrees for rotation
			if (mGizmoType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 45.0f;

			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
				(ImGuizmo::OPERATION)mGizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
				nullptr, snap ? snapValues : nullptr);

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation, rotation, scale;
				Math::DecomposeTransform(transform, translation, rotation, scale);

				glm::vec3 deltaRotation = rotation - tc.rotation;
				tc.translation = translation;
				tc.rotation += deltaRotation;
				tc.scale = scale;
			}
		}
	}

	void EditorLayer::UI_MenuBar()
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Create Project"))
				{
					ModalManager::Open<NewProjectModal>("New Project...", ModalButtons::OKCancel, mEditorData);
					ModalManager::AddOnComplete([&]()
					{
						if (!mEditorData.projectFilepath.empty())
							CreateProject(mEditorData.projectFilepath / mEditorData.projectName);
					});
				}
				if (ImGui::MenuItem("Open Project", "Ctrl+O"))
					OpenProject();
				if (ImGui::MenuItem("Save Project"))
					SaveProject();
				if (Project::GetActive() && ImGui::MenuItem("Project Settings"))
					ModalManager::Open<ProjectSettingsModal>("Project Settings", ModalButtons::OK, Project::GetActive());

				ImGui::Separator();

				if (ImGui::MenuItem("New Scene", "Ctrl+N"))
					NewScene();
				if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
					SaveScene();
				if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S"))
					SaveSceneAs();

				ImGui::Separator();

				if (ImGui::MenuItem("Preferences", "Ctrl+P"))
					ModalManager::Open<SettingsModal>("Project Settings", ModalButtons::OKCancel);

				ImGui::Separator();

				if (ImGui::MenuItem("Exit")) Application::Close();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View"))
			{
				for (auto& [key, _panel, display] : PanelManager::GetPanels())
					ImGui::MenuItem(key.c_str(), nullptr, &display);

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
	}

	void EditorLayer::UI_ChildPanels()
	{
		PanelManager::RenderPanels();
	}

	void EditorLayer::UI_Toolbar()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		auto& colours = ImGui::GetStyle().Colors;
		const auto& buttonHovered = colours[ImGuiCol_ButtonHovered];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
		const auto& buttonActive = colours[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

		ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		float size = ImGui::GetWindowHeight() - 4.0f;
		{
			Ref<Texture2D> icon = (mSceneState == SceneState::Edit || mSceneState == SceneState::Simulate) ? EditorResources::PlayIcon : EditorResources::StopIcon;
			ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
			if (ImGui::ImageButton((ImTextureID)(intptr_t)icon->getRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
			{
				if (mSceneState == SceneState::Edit)
					OnScenePlay();
				else if (mSceneState == SceneState::Play)
					OnSceneStop();
			}
		}
		ImGui::SameLine();
		{
			Ref<Texture2D> icon = (mSceneState == SceneState::Edit || mSceneState == SceneState::Play) ? EditorResources::SimulateIcon : EditorResources::StopIcon;
			if (ImGui::ImageButton((ImTextureID)(intptr_t)icon->getRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
			{
				if (mSceneState == SceneState::Edit)
					OnSceneSimulate();
				else if (mSceneState == SceneState::Simulate)
					OnSceneStop();
			}
		}

		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);
		ImGui::End();
	}

	void EditorLayer::onEvent(Event& e)
	{
		if (mSceneState == SceneState::Edit || mSceneState == SceneState::Simulate)
			mEditorData.camera.onEvent(e);

		PanelManager::DispatchEvents(e);

		EventDispatcher dispatcher(e);
		dispatcher.dispatch<KeyPressedEvent>(LAB_BIND_EVENT_FUNC(EditorLayer::OnKeyPressed));
		dispatcher.dispatch<MouseButtonPressedEvent>(LAB_BIND_EVENT_FUNC(EditorLayer::OnMouseButtonPressed));
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.isRepeated())
			return false;

		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);

		switch (e.getKeyCode())
		{
			case Key::N:
			{
				if (control)
					NewScene();
			}
			break;
			case Key::O:
			{
				if (control)
					OpenProject();

			}
			break;
			case Key::P:
			{
				if (control)
					ModalManager::Open<SettingsModal>("SettingsModal", ModalButtons::OKCancel);

			}
			break;
			case Key::S:
			{
				if (control && shift)
					SaveSceneAs();
				else if (control)
					SaveScene();
			}
			break;

			// Scene Commands
			case Key::D:
			{
				if (control)
					CloneEntity();
			}
			break;
			case Key::Space:
			{
				switch (mSceneState)
				{
				case SceneState::Edit:
				{
					if (control) OnScenePlay();
					else OnSceneSimulate();
					break;
				}
				default:
				{
					OnSceneStop();
					break;
				}
				}
			}
			break;

			// Gizmos
			case Key::Q:
			{
				if (!ImGuizmo::IsUsing())
					mGizmoType = -1;
			}
			break;
			case Key::W:
			{
				if (!ImGuizmo::IsUsing())
					mGizmoType = ImGuizmo::OPERATION::TRANSLATE;
				break;
			}
			case Key::E:
			{
				if (!ImGuizmo::IsUsing())
					mGizmoType = ImGuizmo::OPERATION::ROTATE;
				break;
			}
			case Key::R:
			{
				if (!ImGuizmo::IsUsing())
					mGizmoType = ImGuizmo::OPERATION::SCALE;
				break;
			}
		}


		return false;
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		switch (e.getMouseButton())
		{
		case Mouse::ButtonLeft:
			{
				if (mViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt))
					mScenePanel->setSelectedEntity(mHoveredEntity);
			}
		}

		return false;
	}

	void EditorLayer::LoadSettings()
	{
		JsonObj settings = JSON::Open("enigma.ini");
		if (settings.empty())
			return;

		// Panels
		if (settings.contains("Panels"))
		{
			for (const JsonObj& panel : settings["Panels"])
			{
				if (PanelItem* panelItem = PanelManager::GetPanelItem(panel["Key"]))
					panelItem->displayed = panel["Displayed"];
			}
		}
	}

	void EditorLayer::WriteSettings()
	{
		Application::WriteSettings("enigma.ini");

		JsonObj settings = JSON::Open("enigma.ini");

		settings["Panels"] = {};

		// Panels
		for (const PanelItem& panelItem : PanelManager::GetPanels())
		{
			JsonObj panel;
			panel["Key"] = panelItem.key;
			panel["Displayed"] = panelItem.displayed;

			settings["Panels"].push_back(panel);
		}

		JSON::Write("enigma.ini", settings);
	}

	void EditorLayer::CreateProject(const fs::path& filepath)
	{
		if (!fs::exists(filepath))
			FileUtils::CreateDir(filepath);

		FileUtils::CopyDir("resources/new-project-template", filepath);
		{
			// premake5.lua
			std::string str;
			FileUtils::Read(filepath / "premake5.lua", str);
			ReplaceToken(str, "$PROJECT_NAME$", mEditorData.projectName);

			FileUtils::Write(filepath / "premake5.lua", str);
		}

		{
			// Project File
			std::string str;
			FileUtils::Read(filepath / "project.lpj", str);
			ReplaceToken(str, "$PROJECT_NAME$", mEditorData.projectName);

			FileUtils::Write(filepath / "project.lpj", str);

			std::string newProjectFileName = mEditorData.projectName + ".lpj";
			fs::rename(filepath / "project.lpj", filepath / newProjectFileName);
		}

		FileUtils::CreateDir(filepath / "assets" / "scenes");
		FileUtils::CreateDir(filepath / "assets" / "scripts" / "Source");
		FileUtils::CreateDir(filepath / "assets" / "textures");
		FileUtils::CreateDir(filepath / "assets" / "spritesheets");
		FileUtils::CreateDir(filepath / "assets" / "tilemaps");

		EditorLayer::RegenScriptProject(filepath);

		OpenProject(filepath.string() + "/" + mEditorData.projectName + ".lpj");
	}

	void EditorLayer::OpenProject()
	{
		fs::path filepath = FileUtils::OpenFile({ "Labyrinth Project (*.lpj)", "*.lpj" });

		if (filepath.empty())
			return;

		OpenProject(filepath);
	}

	void EditorLayer::OpenProject(const fs::path& filepath)
	{
		if (!fs::exists(filepath))
		{
			LAB_CORE_ERROR("Tried to open a project that doesn't exist. Project path: {0}", filepath);
			mEditorData.projectFilepath = fs::path();
			return;
		}

		if (Project::GetActive())
			CloseProject();

		Ref<Project> project = Ref<Project>::Create();
		ProjectSerialiser serialiser(project);
		serialiser.deserialise(filepath);
		Project::SetActive(project);

		auto appAssemblyPath = Project::GetScriptModuleFilePath();
		if (!appAssemblyPath.empty() && fs::exists(appAssemblyPath))
			ScriptEngine::LoadAppAssembly();
		else
			LAB_WARN("No C# assembly has been provided in the Project Settings, or it wasn't found.");

		PanelManager::ProjectChanged(project);

		if (!project->getSettings().startScenePath.empty())
			OpenScene(Project::GetProjectDirectory() / project->getSettings().startScenePath);
		else
			NewScene();

		mScenePanel->setSelectedEntity({});

		mEditorData.camera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);

		mEditorData.projectName = std::string();
		mEditorData.projectFilepath = fs::path();
	}

	void EditorLayer::SaveProject()
	{
		if (!Project::GetActive())
			return;

		auto& project = Project::GetActive();
		ProjectSerialiser serialiser(project);
		serialiser.serialise(project->getSettings().projectDir / project->getSettings().projectName);
	}

	void EditorLayer::CloseProject(bool unloadProject)
	{
		SaveProject();

		ScriptEngine::UnloadAppAssembly();
		ScriptEngine::SetContext(nullptr);

		mCurrentScene = nullptr;

		// Check that mEditorScene is the last one (so setting it null here will destroy the scene)
		if (mEditorScene->getRefCount() != 1)
			LAB_CORE_ERROR("Scene will not be destroyed after project is closed - something is still holding scene refs!");
		mEditorScene = nullptr;
		
		if (unloadProject)
			Project::SetActive(nullptr);
	}

	void EditorLayer::NewScene()
	{
		if (mSceneState != SceneState::Edit)
			return;

		mEditorScene = Ref<Scene>::Create();
		mEditorData.currentFile = std::string();
		SetCurrentScene(mEditorScene);
	}

	bool EditorLayer::OpenScene()
	{
		mEditorData.currentFile = FileUtils::OpenFile({ "Labyrinth Scene (.lscene)", "*.lscene"}).string();
		if (!mEditorData.currentFile.empty())
			return OpenScene(mEditorData.currentFile);

		return false;
	}

	bool EditorLayer::OpenScene(const fs::path& path)
	{
		if (mSceneState != SceneState::Edit)
			OnSceneStop();

		if (!AssetManager::IsExtensionValid(path.extension().string(), AssetType::Scene))
		{
			LAB_WARN("Could not load {0} - not a scene file", path.filename().string());
			return false;
		}

		Ref<Scene> newScene = AssetManager::GetAsset<Scene>(path);
		if (!newScene)
			return false;

		mEditorScene = newScene;
		mEditorData.currentFile = path.string();
		if (!mEditorScene->hasName())
			mEditorScene->setName(path.stem().string());

		SetCurrentScene(mEditorScene);
		return true;
	}

	void EditorLayer::SaveScene()
	{
		SyncWindowTitle();

		if (!mEditorData.currentFile.empty())
		{
			SceneSerialiser serialiser(mCurrentScene);
			serialiser.serialise(mEditorData.currentFile);
		}
		else SaveSceneAs();
	}

	void EditorLayer::SaveSceneAs()
	{
		SyncWindowTitle();

		mEditorData.currentFile = FileUtils::SaveFile({ "Labyrinth Scene (.lscene)", "*.lscene"}).string();
		if (!mEditorData.currentFile.empty())
		{
			SceneSerialiser serialiser(mCurrentScene);
			serialiser.serialise(mEditorData.currentFile);
		}
	}
	
	void EditorLayer::OnScenePlay()
	{
		mSceneState = SceneState::Play;

		mCurrentScene = mEditorScene->Clone();
		mCurrentScene->onRuntimeStart();

		mScenePanel->setContext(mCurrentScene);
	}

	void EditorLayer::OnSceneSimulate()
	{
		mSceneState = SceneState::Simulate;

		mCurrentScene = mEditorScene->Clone();
		mCurrentScene->onSimulationStart();

		mScenePanel->setContext(mCurrentScene);
	}

	void EditorLayer::OnSceneStop()
	{
		mSceneState = SceneState::Edit;

		mCurrentScene->onRuntimeStop();
		mCurrentScene = mEditorScene;

		mScenePanel->setContext(mCurrentScene);
	}

	void EditorLayer::OnOverlayRender()
	{
		switch (mSceneState)
		{
		case SceneState::Edit:
			Renderer2D::BeginState(mEditorData.camera);
			break;

		case SceneState::Simulate:
			Renderer2D::BeginState(mEditorData.camera);
			break;

		case SceneState::Play:
		{
			Entity camera = mCurrentScene->getPrimaryCameraEntity();
			if (!camera) return;

			Renderer2D::BeginState(camera.getComponent<CameraComponent>().camera, camera.getComponent<TransformComponent>());
			break;
		}
		default:
			return;
		}

		if (mEditorData.displayColliders)
		{
			mCurrentScene->getEntitiesWith<TransformComponent, BoxColliderComponent>().each([this](const auto& tc, const auto& bcc)
			{
				glm::vec3 scale = tc.scale * glm::vec3(bcc.halfExtents * 2.0f, 1.0f);

				glm::mat4 transform = glm::translate(glm::mat4(1.0f), tc.translation)
					* glm::rotate(glm::mat4(1.0f), tc.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f))
					* glm::translate(glm::mat4(1.0f), glm::vec3(bcc.offset, 0.001f))
					* glm::scale(glm::mat4(1.0f), scale);

				Renderer2D::DrawRect(transform, mEditorData.colliderColour);
			});

			mCurrentScene->getEntitiesWith<TransformComponent, CircleColliderComponent>().each([this](const auto& tc, const auto& ccc)
			{
				glm::vec3 scale = tc.scale * glm::vec3(ccc.radius * 2.0f);

				glm::mat4 transform = glm::translate(glm::mat4(1.0f), tc.translation)
					* glm::translate(glm::mat4(1.0f), glm::vec3(ccc.offset, 0.001f))
					* glm::scale(glm::mat4(1.0f), scale);

				Renderer2D::DrawCircle(transform, mEditorData.colliderColour, 0.05f);
			});
		}

		if (Entity selectedEntity = mScenePanel->getSelectedEntity())
		{
			if (selectedEntity.hasComponent<TransformComponent>())
			{
				const auto& transform = selectedEntity.getComponent<TransformComponent>();

				Renderer2D::DrawRect(transform.getTransform(), mEditorData.selectionColour);
			}
		}

		Renderer2D::EndState();
	}

	void EditorLayer::SetCurrentScene(const Ref<Scene>& currentScene)
	{
		LAB_ASSERT(currentScene, "EditorLayer CurrentScene cannot be null");

		mCurrentScene = currentScene;
		mCurrentScene->onViewportResize((u32)mViewportSize.x, (u32)mViewportSize.y);

		mScenePanel->setContext(mCurrentScene);

		SyncWindowTitle();
	}

	void EditorLayer::SyncWindowTitle()
	{
		std::string title = "Enigma";
		title += " - " + mCurrentScene->getName();
		title += " (" + (mEditorData.currentFile.empty() ? "unsaved" : mEditorData.currentFile) + ")";

		Application::Get().getWindow().setTitle(title);
	}

	void EditorLayer::CloneEntity()
	{
		if (mSceneState != SceneState::Edit)
			return;

		Entity selectedEntity = mScenePanel->getSelectedEntity();
		if (selectedEntity)
			mCurrentScene->CloneEntity(selectedEntity);
	}

	void EditorLayer::RegenScriptProject(const fs::path& filepath)
	{
		static std::string sEnvVarString;

		fs::path rootDirectory = std::filesystem::absolute("./resources").parent_path();
		std::string rootDirectoryString = rootDirectory.string();

		if (rootDirectory.stem().string() == "Enigma")
			rootDirectoryString = rootDirectory.parent_path().string();
		sEnvVarString = "LAB_ROOT_DIR=" + rootDirectoryString;

		std::string batchFilePath = filepath.string();
		std::replace(batchFilePath.begin(), batchFilePath.end(), '/', '\\'); // Only windows
		batchFilePath += "\\Win-CreateScriptProjects.bat";

#ifdef LAB_PLATFORM_WINDOWS
		int error = _putenv(sEnvVarString.c_str());
#elif defined(LAB_PLATFORM_LINUX)
		int error = putenv(sEnvVarString.c_str());
#else
		int error = -1;
#endif
		if (error)
			LAB_CORE_ERROR("Could not set the Labyrinth root directory!");
		system(batchFilePath.c_str());
	}

}