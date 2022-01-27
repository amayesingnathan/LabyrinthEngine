#include "EditorLayer.h"

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
		:	Layer("EditorLayer"), mSquareColour({ 0.5f, 0.15f, 0.15f, 1.0f })
	{
	}

	void EditorLayer::onAttach()
	{
		LAB_PROFILE_FUNCTION();

		mCheckerboardTexture = Texture2D::Create("assets/textures/checkerboard.png");
		mIconPlay = Texture2D::Create("resources/icons/playbutton.png");
		mIconStop = Texture2D::Create("resources/icons/stopbutton.png");

		FramebufferSpec fbSpec;
		fbSpec.width = 1600;
		fbSpec.height = 900;
		fbSpec.attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.samples = 1;
			
		mFramebuffer = Framebuffer::Create(fbSpec);

		mCurrentScene = CreateRef<Scene>();

		mEditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);

		mScenePanel.setContext(mCurrentScene);

	}

	void EditorLayer::onDetach()
	{
		LAB_PROFILE_FUNCTION();
	}

	void EditorLayer::onUpdate(Timestep ts)
	{
		LAB_PROFILE_FUNCTION();

		if (FramebufferSpec spec = mFramebuffer->getSpecification();
			mViewportSize.x > 0.0f && mViewportSize.y > 0.0f && // zero sized framebuffer is invalid
			(spec.width != mViewportSize.x || spec.height != mViewportSize.y))
		{
			mFramebuffer->resize((uint32_t)mViewportSize.x, (uint32_t)mViewportSize.y);

			mEditorCamera.setViewportSize(mViewportSize.x, mViewportSize.y);
			mCurrentScene->onViewportResize((uint32_t)mViewportSize.x, (uint32_t)mViewportSize.y);
		}

		Renderer2D::ResetStats();

		mFramebuffer->bind();
		RenderCommand::SetClearColor({ 0.125f, 0.0625f, 0.25f, 1.0f });
		RenderCommand::Clear();

		// Clear our entity ID attachment to -1
		mFramebuffer->clearAttachment(1, -1);

		switch (mSceneState)
		{
		case SceneState::Edit:
		{
			mEditorCamera.onUpdate(ts);

			mCurrentScene->onUpdateEditor(ts, mEditorCamera);
			break;
		}
		case SceneState::Play:
		{
			mCurrentScene->onUpdateRuntime(ts);
			break;
		}
		}

		auto [mx, my] = ImGui::GetMousePos();
		mx -= mViewportBounds[0].x;
		my -= mViewportBounds[0].y;
		glm::vec2 viewportSize = mViewportBounds[1] - mViewportBounds[0];
		my = viewportSize.y - my;
		int mouseX = (int)mx;
		int mouseY = (int)my;

		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
		{
			int pixelData = mFramebuffer->readPixel(1, mouseX, mouseY); 
			mHoveredEntity = (pixelData == -1) ? Entity() : Entity((entt::entity)pixelData, mCurrentScene);
		}

		mFramebuffer->unbind();
		
	}

	void EditorLayer::onImGuiRender()
	{
		LAB_PROFILE_FUNCTION();

		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
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

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
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

				if (ImGui::MenuItem("Exit")) Application::Get().Close();
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		mScenePanel.onImGuiRender();
		mContentBrowserPanel.onImGuiRender();
		mSpriteSheetPanel.onImGuiRender();

		ImGui::Begin("Stats");

		std::string name = "None";
		if (mHoveredEntity)
			name = mHoveredEntity.getComponent<TagComponent>().tag;
		ImGui::Text("Hovered Entity: %s", name.c_str());

		auto stats = Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.drawCalls);
		ImGui::Text("Quads: %d", stats.quadCount);
		ImGui::Text("Vertices: %d", stats.getTotalVertexCount());
		ImGui::Text("Indices: %d", stats.getTotalIndexCount());

		ImGui::End();

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

		uint32_t textureID = mFramebuffer->getColorAttachmentRendererID();
		ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ mViewportSize.x, mViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				std::filesystem::path fullPath = (std::filesystem::path(gAssetPath) / path).string();
				if (fullPath.extension().string() == ".labr")
					OpenScene();
			}
			ImGui::EndDragDropTarget();
		}

		// Gizmos
		Entity selectedEntity = mScenePanel.getSelectedEntity();
		if (selectedEntity && mGizmoType != -1 && mViewportFocused)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			float windowWidth = (float)ImGui::GetWindowWidth();
			float windowHeight = (float)ImGui::GetWindowHeight();
			ImGuizmo::SetRect(mViewportBounds[0].x, mViewportBounds[0].y, mViewportBounds[1].x - mViewportBounds[0].x, mViewportBounds[1].y - mViewportBounds[0].y);

			// Runtime camera from entity
			//auto cameraEntity = mCurrentScene->getPrimaryCameraEntity();
			//const auto& camera = cameraEntity.getComponent<CameraComponent>().camera;
			//const glm::mat4& cameraProjection = camera.getProjection();
			//glm::mat4 cameraView = glm::inverse(cameraEntity.getComponent<TransformComponent>().getTransform());

			//Editor camera
			const glm::mat4& cameraProjection = mEditorCamera.getProjection();
			glm::mat4 cameraView = mEditorCamera.getViewMatrix();

			// Entity transform
			auto& tc = selectedEntity.getComponent<TransformComponent>();
			glm::mat4 transform = tc.getTransform();

			// Snapping
			bool snap = Input::IsKeyPressed(LAB_KEY_LCTRL);
			float snapValue = 0.5f; // Snap to 0.5m for translation/scale
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


		ImGui::End();
		ImGui::PopStyleVar();

		UI_Toolbar();

		ImGui::End();

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
		Ref<Texture2D> icon = mSceneState == SceneState::Edit ? mIconPlay : mIconStop;
		ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
		if (ImGui::ImageButton((ImTextureID)icon->getRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
		{
			if (mSceneState == SceneState::Edit)
				OnScenePlay();
			else if (mSceneState == SceneState::Play)
				OnSceneStop();
		}
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);
		ImGui::End();
	}

	void EditorLayer::onEvent(Event& e)
	{
		mEditorCamera.onEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.dispatch<KeyPressedEvent>(LAB_BIND_EVENT_FUNC(EditorLayer::OnKeyPressed));
		dispatcher.dispatch<MouseButtonPressedEvent>(LAB_BIND_EVENT_FUNC(EditorLayer::OnMouseButtonPressed));
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.getRepeatCount() > 0)
			return false;

		bool control = Input::IsKeyPressed(LAB_KEY_LCTRL) || Input::IsKeyPressed(LAB_KEY_RCTRL);
		bool shift = Input::IsKeyPressed(LAB_KEY_LSHIFT) || Input::IsKeyPressed(LAB_KEY_RSHIFT);

		switch (e.getKeyCode())
		{
			case LAB_KEY_N:
			{
				if (control)
					NewScene();

				break;
			}
			case LAB_KEY_O:
			{
				if (control)
					OpenScene();

				break;
			}
			case LAB_KEY_S:
			{
				if (control && shift)
					SaveSceneAs();
				else if (control)
					SaveScene();

				break;
			}


			// Gizmos
			case LAB_KEY_Q:
			{
				if (!ImGuizmo::IsUsing())
					mGizmoType = -1;
				break;
			}
			case LAB_KEY_W:
			{
				if (!ImGuizmo::IsUsing())
					mGizmoType = ImGuizmo::OPERATION::TRANSLATE;
				break;
			}
			case LAB_KEY_E:
			{
				if (!ImGuizmo::IsUsing())
					mGizmoType = ImGuizmo::OPERATION::ROTATE;
				break;
			}
			case LAB_KEY_R:
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
			case LAB_MOUSE_BUTTON_LEFT:
			{
				if (mViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(LAB_KEY_LALT))
					mScenePanel.setSelectedEntity(mHoveredEntity);
			}
		}

		return false;
	}

	void EditorLayer::NewScene()
	{
		mCurrentScene = CreateRef<Scene>();
		mCurrentScene->onViewportResize((uint32_t)mViewportSize.x, (uint32_t)mViewportSize.y);
		mScenePanel.setContext(mCurrentScene);
	}

	void EditorLayer::OpenScene()
	{
		mFilepath = FileDialogs::OpenFile("Labyrinth Scene (*.laby)\0*.laby\0" "Labyrinth Entity(*.lbent)\0* .lbent\0");
		if (!mFilepath.empty())
		{
			OpenScene(mFilepath);
		}
	}

	void EditorLayer::OpenScene(const std::filesystem::path& path)
	{
		mCurrentScene = CreateRef<Scene>();
		mCurrentScene->onViewportResize((uint32_t)mViewportSize.x, (uint32_t)mViewportSize.y);
		mScenePanel.setContext(mCurrentScene);

		Serialiser::Deserialise<Scene>(path.string(), mCurrentScene);

		mFilepath = path.string();
	}

	void EditorLayer::SaveScene()
	{
		if (!mFilepath.empty())
		{
			Serialiser::Serialise(mCurrentScene, mFilepath);
		}
		else SaveSceneAs();
	}

	void EditorLayer::SaveSceneAs()
	{
		mFilepath = FileDialogs::SaveFile("Labyrinth Scene (*.laby)\0*.laby\0" "Labyrinth Entity(*.lbent)\0* .lbent\0");
		if (!mFilepath.empty())
		{
			Serialiser::Serialise(mCurrentScene, mFilepath);
		}
	}
	
	void EditorLayer::OnScenePlay()
	{
		mSceneState = SceneState::Play;
	}

	void EditorLayer::OnSceneStop()
	{
		mSceneState = SceneState::Edit;

	}
}