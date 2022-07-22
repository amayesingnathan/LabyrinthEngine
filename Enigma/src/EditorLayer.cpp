#include "EditorLayer.h"

#include "Modals/ModalManager.h"
#include "Modals/SettingsModal.h"

#include "Panels/ScenePanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Panels/AssetPanel.h"
#include "Panels/SpriteSheetPanel.h"
#include "Panels/OptionsPanel.h"
#include "Panels/StatsPanel.h"

#include "Labyrinth/Assets/AssetManager.h"
#include "Labyrinth/Assets/AssetGroup.h"

#include "Labyrinth/IO/Input.h"

#include "Labyrinth/Renderer/Renderer2D.h"
#include "Labyrinth/Renderer/RenderCommand.h"

#include "Labyrinth/Scene/Serialiser.h"

#include "Labyrinth/Tools/PlatformUtils.h"

#include "Labyrinth/Maths/Maths.h"

#include "imgui/imgui.h"
#include "ImGuizmo.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Labyrinth {

	extern const std::filesystem::path gAssetPath;

	EditorLayer::EditorLayer()
		:	Layer("EditorLayer")
	{
	}

	void EditorLayer::onAttach()
	{
		LAB_PROFILE_FUNCTION();

		Ref<Tex2DGroup> iconGroup = AssetManager::CreateAsset<Tex2DGroup>("Icons.ltg", "Icons/", StorageType::Map);
		//mHighlight = iconGroup->add("Highlight", "resources/icons/highlight.png");
		mIconPlay = iconGroup->add("Play", "resources/icons/playbutton.png");
		mIconStop = iconGroup->add("Stop", "resources/icons/stopbutton.png");
		mIconSim = iconGroup->add("Sim", "resources/icons/simbutton.png");

		FramebufferSpec fbSpec;
		fbSpec.width = 1600;
		fbSpec.height = 900;
		fbSpec.attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.samples = 1;
			
		mFramebuffer = Framebuffer::Create(fbSpec);

		mViewportSize = { fbSpec.width, fbSpec.height };

		mEditorData.camera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);

		mScenePanel = PanelManager::Register<ScenePanel>("Scene Heirarchy", true, mEditorData);
		PanelManager::Register<AssetPanel>("Asset Manager");
		PanelManager::Register<ContentBrowserPanel>("Content Browser");
		PanelManager::Register<SpriteSheetPanel>("Sprite Sheets");
		PanelManager::Register<OptionsPanel>("Options", true, mEditorData);
		PanelManager::Register<StatsPanel>("Statistics", true, mHoveredEntity);

		bool loadedScene = false;
		auto commandLineArgs = Application::Get().getSpec().commandLineArgs;
		if (commandLineArgs.count > 1)
			loadedScene = OpenScene(commandLineArgs[1]);
		if (!loadedScene)
			NewScene();

		LoadSettings();
	}

	void EditorLayer::onDetach()
	{
		LAB_PROFILE_FUNCTION();

		WriteSettings();

		PanelManager::Clear();
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
				std::filesystem::path fullPath = gAssetPath / path;
				if (std::regex_match(fullPath.extension().string(), std::regex(".laby")))
					OpenScene(fullPath);
			}
			ImGui::EndDragDropTarget();
		}

		UI_Gizmos();

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void EditorLayer::UI_Gizmos()
	{
		// Gizmos
		Entity selectedEntity = mScenePanel->getSelectedEntity();
		if (selectedEntity && mGizmoType != -1 && mViewportFocused)
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
				// Disabling fullscreen would allow the window to be moved to the front of other windows, 
				// which we can't undo at the moment without finer window depth/z control.
				//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);
				if (ImGui::MenuItem("New", "Ctrl+N"))
					NewScene();
				if (ImGui::MenuItem("Open", "Ctrl+O"))
					OpenScene();
				if (ImGui::MenuItem("Save", "Ctrl+S"))
					SaveScene();
				if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
					SaveSceneAs();
				if (ImGui::MenuItem("Preferences", "Ctrl+P"))
					ModalManager::Open<SettingsModal>("SettingsModal");

				if (ImGui::MenuItem("Exit")) Application::Get().Close();
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
			Ref<Texture2D> icon = (mSceneState == SceneState::Edit || mSceneState == SceneState::Simulate) ? mIconPlay : mIconStop;
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
			Ref<Texture2D> icon = (mSceneState == SceneState::Edit || mSceneState == SceneState::Play) ? mIconSim : mIconStop;
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
					OpenScene();

			}
			break;
			case Key::P:
			{
				if (control)
					ModalManager::Open<SettingsModal>("SettingsModal");

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
		mEditorData.currentFile = FileDialogs::OpenFile({ "Labyrinth Scene", "*.laby", "Labyrinth Entity", "*.lbent"});
		if (!mEditorData.currentFile.empty())
			return OpenScene(mEditorData.currentFile);

		return false;
	}

	bool EditorLayer::OpenScene(const std::filesystem::path& path)
	{
		if (mSceneState != SceneState::Edit)
			OnSceneStop();

		if (path.extension().string() != ".laby")
		{
			LAB_WARN("Could not load {0} - not a scene file", path.filename().string());
			return false;
		}

		Ref<Scene> newScene = Ref<Scene>::Create();
		if (!Serialiser::Deserialise<Scene>(path.string(), newScene))
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
			Serialiser::Serialise(mCurrentScene, mEditorData.currentFile);
		else SaveSceneAs();
	}

	void EditorLayer::SaveSceneAs()
	{
		SyncWindowTitle();

		mEditorData.currentFile = FileDialogs::SaveFile({ "Labyrinth Scene (.laby)", "*.laby", "Labyrinth Entity (.lent)", "*.lent"});
		if (!mEditorData.currentFile.empty())
			Serialiser::Serialise(mCurrentScene, mEditorData.currentFile);
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
}