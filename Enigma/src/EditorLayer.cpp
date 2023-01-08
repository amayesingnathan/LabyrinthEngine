#include "EditorLayer.h"

#include <imgui.h>
#include "ImGuizmo.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Labyrinth/Core/Application.h>

#include <Labyrinth/Editor/EditorResources.h>
#include <Labyrinth/Editor/ModalManager.h>
#include <Labyrinth/Editor/PanelManager.h>
#include <Labyrinth/Editor/SelectionManager.h>

#include <Labyrinth/Editor/Modals/NewMapModal.h>
#include <Labyrinth/Editor/Modals/NewProjectModal.h>
#include <Labyrinth/Editor/Modals/ProjectSettingsModal.h>
#include <Labyrinth/Editor/Modals/SettingsModal.h>

#include <Labyrinth/IO/Input.h>

#include <Labyrinth/ImGui/ImGuiWidgets.h>

#include <Labyrinth/Project/ProjectSerialiser.h>

#include <Labyrinth/Renderer/Renderer.h>
#include <Labyrinth/Renderer/Renderer2D.h>

#include <Labyrinth/Scene/Components.h>
#include <Labyrinth/Scene/SceneSerialiser.h>

#include <Labyrinth/Tools/MathUtils.h>
#include <Labyrinth/Tools/StringUtils.h>

namespace Laby {

	EditorLayer::EditorLayer()
	{
	}

	void EditorLayer::onAttach()
	{
		EditorResources::Init();

		const ApplicationSpec& appSpec = Application::GetSpec();

		FramebufferSpec fbSpec;
		fbSpec.width = appSpec.resolution.width;
		fbSpec.height = appSpec.resolution.height;
		fbSpec.attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.samples = 1;

		mFramebuffer = Ref<Framebuffer>::Create(fbSpec);

		mEditorData.viewportSize = { fbSpec.width, fbSpec.height };
		mEditorData.camera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);

		LoadSettings();

		if (!appSpec.startupProject.empty())
			OpenProject(appSpec.startupProject);
		else
			NewScene();
	}

	void EditorLayer::onDetach()
	{
		PanelManager::Clear();

		EditorResources::Shutdown();
	}

	void EditorLayer::onUpdate(Timestep ts)
	{
		if (FramebufferSpec spec = mFramebuffer->getSpecification();
			mEditorData.viewportSize.x > 0.0f && mEditorData.viewportSize.y > 0.0f && // zero sized framebuffer is invalid
			(spec.width != mEditorData.viewportSize.x || spec.height != mEditorData.viewportSize.y))
		{
			mFramebuffer->resize((u32)mEditorData.viewportSize.x, (u32)mEditorData.viewportSize.y);

			mEditorData.camera.setViewportSize(mEditorData.viewportSize.x, mEditorData.viewportSize.y);
			mCurrentScene->onViewportResize((u32)mEditorData.viewportSize.x, (u32)mEditorData.viewportSize.y);
		}

		mFramebuffer->bind();
		Renderer::SetClearColor({ 0.125f, 0.0625f, 0.25f, 1.0f });
		Renderer::Clear();

		// Clear our entity ID attachment to -1
		mFramebuffer->clearAttachment(1, -1);

		switch (mSceneState)
		{
		case SceneEdit:
		{
			if (mEditorData.viewportHovered && mEditorData.viewportFocused)
				mEditorData.camera.onUpdate(ts);

			mCurrentScene->onUpdateEditor(ts, mEditorData.camera);
			break;
		}
		case ScenePlay:
		{
			mCurrentScene->onUpdateRuntime(ts);
			break;
		}
		case SceneSimulate:
		{
			if (mEditorData.viewportHovered && mEditorData.viewportFocused)
				mEditorData.camera.onUpdate(ts);

			mCurrentScene->onUpdateSimulation(ts, mEditorData.camera);
			break;
		}
		}

		auto [mx, my] = ImGui::GetMousePos();
		mx -= mEditorData.viewportBounds[0].x;
		my -= mEditorData.viewportBounds[0].y;
		glm::vec2 viewportSize = mEditorData.viewportBounds[1] - mEditorData.viewportBounds[0];
		my = viewportSize.y - my;
		i32 mouseX = (i32)mx;
		i32 mouseY = (i32)my;

		if (mouseX >= 0 && mouseY >= 0 && mouseX < (i32)viewportSize.x && mouseY < (i32)viewportSize.y)
		{
			i32 pixelData = mFramebuffer->readPixel(1, mouseX, mouseY);
			mEditorData.hoveredEntity = (pixelData == -1) ? Entity() : Entity((EntityID)pixelData, mCurrentScene);
		}

		mFramebuffer->unbind();
	}

	void EditorLayer::onImGuiRender()
	{
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;

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
		UI_Toolbar();

		ImGui::End();
	}

	void EditorLayer::onEvent(Event& e)
	{
		if (mEditorData.viewportHovered && (mSceneState == SceneEdit || mSceneState == SceneSimulate))
			mEditorData.camera.onEvent(e);

		PanelManager::DispatchEvents(e);
		ModalManager::DispatchEvents(e);

		LocalEventDispatcher dispatcher(e);
		dispatcher.dispatch<KeyPressedEvent>(LAB_BIND_EVENT_FUNC(OnKeyPressed));
		dispatcher.dispatch<MouseButtonPressedEvent>(LAB_BIND_EVENT_FUNC(OnMouseButtonPressed));
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.repeat)
			return false;

		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
		switch (e.keyCode)
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
				ModalManager::Open<SettingsModal>("Preferences", ModalButtons::OKCancel);

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
		case Key::Escape:
		{
			ModalManager::OpenInline("Quit", ModalButtons::YesNo, 
			[]() {
				ImGui::Text("Are you sure you want to quit? Any unsaved changes will be lost.");
			}, 
			[]() {
				Application::BlockEsc(false);
				Application::Close();
			});
			return true;
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
			case SceneEdit:
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
				mEditorData.gizmoType = -1;
		}
		break;
		case Key::W:
		{
			if (!ImGuizmo::IsUsing())
				mEditorData.gizmoType = ImGuizmo::OPERATION::TRANSLATE;
			break;
		}
		case Key::E:
		{
			if (!ImGuizmo::IsUsing())
				mEditorData.gizmoType = ImGuizmo::OPERATION::ROTATE;
			break;
		}
		case Key::R:
		{
			if (!ImGuizmo::IsUsing())
				mEditorData.gizmoType = ImGuizmo::OPERATION::SCALE;
			break;
		}
		}

		return false;
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);

		switch (e.button)
		{
		case Mouse::ButtonLeft:
		{
			if (mEditorData.viewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt))
			{
				if (!control)
					SelectionManager::DeselectAll(SelectionDomain::Scene);

				if (mEditorData.hoveredEntity)
					SelectionManager::Select(SelectionDomain::Scene, mEditorData.hoveredEntity.getUUID());
			}
			break;
		}
		}

		return false;
	}

	void EditorLayer::OnOverlayRender()
	{
		switch (mSceneState)
		{
		case SceneEdit:
			Renderer2D::BeginState(mEditorData.camera);
			break;

		case SceneSimulate:
			Renderer2D::BeginState(mEditorData.camera);
			break;

		case ScenePlay:
		{
			Entity camera = mCurrentScene->getPrimaryCameraEntity();
			if (!camera) 
				return;

			Renderer2D::BeginState(camera.getComponent<CameraComponent>().camera, camera.getComponent<TransformComponent>());
			break;
		}
		default:
			return;
		}

		if (mEditorData.displayColliders)
		{
			mCurrentScene->getEntitiesWith<TransformComponent, BoxColliderComponent>().each([this](auto entity, const auto& tc, const auto& bcc)
			{
				glm::vec3 scale = tc.scale * glm::vec3(bcc.halfExtents * 2.0f, 1.0f);

				glm::mat4 transform = glm::translate(glm::mat4(1.0f), tc.translation)
					* glm::rotate(glm::mat4(1.0f), tc.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f))
					* glm::translate(glm::mat4(1.0f), glm::vec3(bcc.offset, 0.001f))
					* glm::scale(glm::mat4(1.0f), scale);

				Renderer2D::DrawRect(transform, mEditorData.colliderColour);
			});

			mCurrentScene->getEntitiesWith<TransformComponent, CircleColliderComponent>().each([this](auto entity, const auto& tc, const auto& ccc)
			{
				glm::vec3 scale = tc.scale * glm::vec3(ccc.radius * 2.0f);

				glm::mat4 transform = glm::translate(glm::mat4(1.0f), tc.translation)
					* glm::translate(glm::mat4(1.0f), glm::vec3(ccc.offset, 0.001f))
					* glm::scale(glm::mat4(1.0f), scale);

				Renderer2D::DrawCircle(transform, mEditorData.colliderColour, 0.05f);
			});
		}

		{
			const auto& selection = SelectionManager::GetSelections(SelectionDomain::Scene);
			SelectionManager::ForEach(SelectionDomain::Scene, [this](const UUID& id)
			{
				Entity selectedEntity = mCurrentScene->findEntity(id);
				if (selectedEntity && selectedEntity.hasComponent<TransformComponent>())
				{
					const auto& transform = selectedEntity.getComponent<TransformComponent>();
					Renderer2D::DrawRect(transform.getTransform(), mEditorData.selectionColour);
				}
			});
		}

		Renderer2D::EndState();
	}

	void EditorLayer::UI_Viewport()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");

		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();
		mEditorData.viewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		mEditorData.viewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		mEditorData.viewportFocused = ImGui::IsWindowFocused();
		mEditorData.viewportHovered = ImGui::IsWindowHovered();
		Application::Get().getImGuiLayer()->blockEvents(!mEditorData.viewportFocused && !mEditorData.viewportHovered);

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		mEditorData.viewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		u32 textureID = mFramebuffer->getColourAttachmentRendererID();
		ImGui::Image((ImTextureID)(intptr_t)textureID, ImVec2{ mEditorData.viewportSize.x, mEditorData.viewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

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

		if (mSceneState == SceneEdit) 
			UI_Gizmos();

		ImGui::PopStyleVar();

		ImGui::End();
	}

	void EditorLayer::UI_Gizmos()
	{
		// Gizmos
		const auto& selections = SelectionManager::GetSelections(SelectionDomain::Scene);
		Entity firstSelection = mCurrentScene->findEntity(selections.size() != 0 ? selections[0] : UUID(0));

		if (firstSelection && mEditorData.gizmoType != -1 && mEditorData.viewportFocused && firstSelection.hasComponent<TransformComponent>())
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			float windowWidth = (float)ImGui::GetWindowWidth();
			float windowHeight = (float)ImGui::GetWindowHeight();
			ImGuizmo::SetRect(mEditorData.viewportBounds[0].x, mEditorData.viewportBounds[0].y, mEditorData.viewportBounds[1].x - mEditorData.viewportBounds[0].x, mEditorData.viewportBounds[1].y - mEditorData.viewportBounds[0].y);

			//Editor camera
			const glm::mat4& cameraProjection = mEditorData.camera.getProjection();
			glm::mat4 cameraView = mEditorData.camera.getViewMatrix();

			// Entity transform
			auto& tc = firstSelection.getComponent<TransformComponent>();
			glm::mat4 transform = tc.getTransform();

			// Snapping
			bool snap = Input::IsKeyPressed(Key::LeftControl);
			float snapValue = 1.0f; // Snap to 1.0m for translation/scale
			// Snap to 45 degrees for rotation
			if (mEditorData.gizmoType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 45.0f;

			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
				(ImGuizmo::OPERATION)mEditorData.gizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
				nullptr, snap ? snapValues : nullptr);

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation, rotation, scale;
				MathUtils::DecomposeTransform(transform, translation, rotation, scale);

				glm::vec3 deltaTranslation = translation - tc.translation;
				glm::vec3 deltaRotation = rotation - tc.rotation;
				glm::vec3 deltaScale = scale - tc.scale;

				SelectionManager::ForEach(SelectionDomain::Scene, [&, this](const UUID& id)
				{
					Entity entity = mCurrentScene->findEntity(id);
					if (entity.hasComponent<TransformComponent>())
					{
						TransformComponent& transform = entity.getComponent<TransformComponent>();
						transform.translation += deltaTranslation;
						transform.rotation += deltaRotation;
						transform.scale += deltaScale;
					}
				});
			}
		}
	}

	void EditorLayer::UI_MenuBar()
	{
		UI::MenuBar* menu = Widgets::BeginMenuBar();

		Widgets::AddMenuHeading(menu, "File");
		{	// File Menu
			Widgets::AddMenuItem(menu, "Create Project", [this]()
			{
				ModalManager::Open<NewProjectModal>("New Project...", ModalButtons::OKCancel, mEditorData);
				ModalManager::AddCallback([this]()
				{
					if (!mEditorData.newProjectFilepath.empty())
						CreateProject(mEditorData.newProjectFilepath / mEditorData.newProjectName);
				});
			});
			Widgets::AddMenuItem(menu, "Open Project", "Ctrl+O", [this]() 
			{
				OpenProject();
			});
			Widgets::AddMenuItem(menu, "Save Project", [this]() 
			{
				SaveProject();
			});
			if (Project::IsActive())
			{
				Widgets::AddMenuItem(menu, "Save Project", [this]() 
				{
					ModalManager::Open<ProjectSettingsModal>("Project Settings", ModalButtons::OK, ActiveProject::Get());
				});
			}

			Widgets::AddMenuSeparator(menu);

			Widgets::AddMenuItem(menu, "New Scene", "Ctrl+N", [this]()
			{
				NewScene();
			});
			Widgets::AddMenuItem(menu, "Save Scene", "Ctrl+S", [this]()
			{
				SaveScene();
			});
			Widgets::AddMenuItem(menu, "Save Scene As...", "Ctrl+Shift+S", [this]()
			{
				SaveSceneAs();
			});

			Widgets::AddMenuSeparator(menu);

			Widgets::AddMenuItem(menu, "Preferences", "Ctrl+P", [this]()
			{
				ModalManager::Open<SettingsModal>("Settings", ModalButtons::OKCancel);
			});

			Widgets::AddMenuSeparator(menu);

			Widgets::AddMenuItem(menu, "Save Scene As...", "Ctrl+Shift+S", [this]()
			{
				Application::Close();
			});
		}
		
		Widgets::AddMenuHeading(menu, "View");
		{	// View 
			for (auto& [key, _panel, display] : PanelManager::GetPanels())
				Widgets::AddMenuItem(menu, key, display);
		}

		Widgets::AddMenuHeading(menu, "Create");
		{	// Create 

			Widgets::AddMenuItem(menu, "Tilemap", [this]()
			{
				ModalManager::Open<NewMapModal>("New Tilemap...", ModalButtons::Custom);
			});
		}

		Widgets::EndMenuBar(menu);
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
			Ref<Texture2D> icon = (mSceneState == SceneEdit || mSceneState == SceneSimulate) ? EditorResources::PlayIcon : EditorResources::StopIcon;
			Widgets::SetXPosition((Widgets::GetAvailableRegion().x * 0.5f) - (size * 0.5f));
			Widgets::ImageButton(icon, { size, size }, [this]()
			{
				if (mSceneState == SceneEdit)
					OnScenePlay();
				else if (mSceneState == ScenePlay)
					OnSceneStop();
			});
		}
		ImGui::SameLine();
		{
			Ref<Texture2D> icon = (mSceneState == SceneEdit || mSceneState == ScenePlay) ? EditorResources::SimulateIcon : EditorResources::StopIcon;
			Widgets::ImageButton(icon, { size, size }, [this]()
			{
				if (mSceneState == SceneEdit)
				OnSceneSimulate();
				else if (mSceneState == ScenePlay)
					OnSceneStop();
			});
		}

		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);
		ImGui::End();
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
				if (auto* panelItem = PanelManager::Find(panel["Key"]))
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
		for (const auto& panelItem : PanelManager::GetPanels())
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
			StringUtils::ReplaceToken(str, "$PROJECT_NAME$", mEditorData.newProjectName);

			FileUtils::Write(filepath / "premake5.lua", str);
		}

		{
			// Project File
			std::string str;
			FileUtils::Read(filepath / "project.lpj", str);
			StringUtils::ReplaceToken(str, "$PROJECT_NAME$", mEditorData.newProjectName);

			FileUtils::Write(filepath / "project.lpj", str);

			std::string newProjectFileName = mEditorData.newProjectName + ".lpj";
			fs::rename(filepath / "project.lpj", filepath / newProjectFileName);
		}

		FileUtils::CreateDir(filepath / "assets" / "scenes");
		FileUtils::CreateDir(filepath / "assets" / "scripts" / "Source");
		FileUtils::CreateDir(filepath / "assets" / "textures");
		FileUtils::CreateDir(filepath / "assets" / "spritesheets");
		FileUtils::CreateDir(filepath / "assets" / "tilemaps");

		EditorLayer::RegenScriptProject(filepath);

		OpenProject(filepath.string() + "/" + mEditorData.newProjectName + ".lpj");
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
			mEditorData.newProjectFilepath = fs::path();
			return;
		}

		if (Project::IsActive())
			CloseProject();

		Ref<Project> project = Ref<Project>::Create();
		ProjectSerialiser serialiser(project);
		serialiser.deserialise(filepath);
		ActiveProject::Set(project);

		auto appAssemblyPath = Project::GetScriptModuleFilePath();
		if (!appAssemblyPath.empty() && fs::exists(appAssemblyPath))
			ScriptEngine::LoadAppAssembly();
		else
			LAB_WARN("No C# assembly has been provided in the Project Settings, or it wasn't found.");

		PanelManager::ProjectChanged(project);

		const fs::path& startScenePath = Project::GetStartScenePath();
		if (!startScenePath.empty())
			OpenScene(Project::GetProjectDirectory() / startScenePath);
		else
			NewScene();

		SelectionManager::DeselectAll();

		mEditorData.camera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);

		mEditorData.newProjectName = std::string();
		mEditorData.newProjectFilepath = fs::path();
	}

	void EditorLayer::SaveProject()
	{
		if (!Project::IsActive())
			return;

		ProjectSerialiser serialiser;
		serialiser.serialise(Project::GetProjectPath());
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
			Project::SetInactive();
	}

	void EditorLayer::NewScene()
	{
		if (mSceneState != SceneEdit)
			return;

		mEditorScene = Ref<Scene>::Create();
		mEditorData.currentFile = std::string();
		SetCurrentScene(mEditorScene);
	}

	bool EditorLayer::OpenScene()
	{
		mEditorData.currentFile = FileUtils::OpenFile({ "Labyrinth Scene (.lscene)", "*.lscene" }).string();
		if (!mEditorData.currentFile.empty())
			return OpenScene(mEditorData.currentFile);

		return false;
	}

	bool EditorLayer::OpenScene(const fs::path& path)
	{
		if (mSceneState != SceneEdit)
			OnSceneStop();

		if (!AssetManager::IsExtensionValid(path.extension().string(), AssetType::Scene))
		{
			LAB_WARN("Could not load {0} - not a scene file", path.filename().string());
			return false;
		}

		Ref<Scene> newScene = Ref<Scene>::Create("Untitled");
		SceneSerialiser serialiser(newScene);
		serialiser.deserialise(path);

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

		mEditorData.currentFile = FileUtils::SaveFile({ "Labyrinth Scene (.lscene)", "*.lscene" }).string();
		if (!mEditorData.currentFile.empty())
		{
			SceneSerialiser serialiser(mCurrentScene);
			serialiser.serialise(mEditorData.currentFile);
		}
	}

	void EditorLayer::OnScenePlay()
	{
		mCurrentScene = SceneUtils::Clone(mEditorScene);
		mCurrentScene->onRuntimeStart();

		mSceneState = ScenePlay;
		//mScenePanel->setContext(mCurrentScene);
	}

	void EditorLayer::OnSceneSimulate()
	{
		mCurrentScene = SceneUtils::Clone(mEditorScene);
		mCurrentScene->onSimulationStart();

		mSceneState = SceneSimulate;
		//mScenePanel->setContext(mCurrentScene);
	}

	void EditorLayer::OnSceneStop()
	{
		switch (mSceneState)
		{
		case ScenePlay: mCurrentScene->onRuntimeStop(); break;
		case SceneSimulate: mCurrentScene->onSimulationStop(); break;
		case SceneEdit: break;
		}

		mCurrentScene = mEditorScene;

		mSceneState = SceneEdit;
		//mScenePanel->setContext(mCurrentScene);
	}

	void EditorLayer::SetCurrentScene(const Ref<Scene>& currentScene)
	{
		LAB_ASSERT(currentScene, "EditorLayer CurrentScene cannot be null");

		mCurrentScene = currentScene;
		mCurrentScene->onViewportResize((u32)mEditorData.viewportSize.x, (u32)mEditorData.viewportSize.y);

		//mScenePanel->setContext(mCurrentScene);

		SelectionManager::DeselectAll(SelectionDomain::Scene);

		SyncWindowTitle();
	}

	void EditorLayer::SyncWindowTitle()
	{
		std::string title = fmt::format("Enigma - {0} ({1})", mCurrentScene->getName(), (mEditorData.currentFile.empty() ? "unsaved" : mEditorData.currentFile));

		Application::Get().getWindow().setTitle(title);
	}

	void EditorLayer::CloneEntity()
	{
		if (mSceneState != SceneEdit)
			return;

		const auto& selections = SelectionManager::GetSelections(SelectionDomain::Scene);
		Entity firstSelection = mCurrentScene->findEntity(selections.size() != 0 ? selections[0] : UUID(0));
		if (firstSelection)
			mCurrentScene->CloneEntity(firstSelection);
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
		LAB_STATIC_ASSERT("Platform definition required for system environment variable call")
#endif
		if (error)
		{
			LAB_CORE_ERROR("Could not set the Labyrinth root directory!");
			return;
		}
		system(batchFilePath.c_str());
	}
}