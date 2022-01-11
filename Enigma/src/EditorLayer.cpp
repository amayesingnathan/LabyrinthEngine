#include "EditorLayer.h"

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Labyrinth/Scene/SceneSerialiser.h"

#include "Labyrinth/Tools/PlatformUtils.h"

namespace Labyrinth {

	EditorLayer::EditorLayer()
		:	Layer("EditorLayer"), mCameraController(1280.0f / 720.0f, true), mSquareColour({ 0.5f, 0.15f, 0.15f, 1.0f })
	{
	}

	void EditorLayer::onAttach()
	{
		LAB_PROFILE_FUNCTION();

		mCheckerboardTexture = Texture2D::Create("assets/textures/checkerboard.png");

		FramebufferSpec fbSpec;
		fbSpec.width = 1600;
		fbSpec.height = 900;
		mFramebuffer = Framebuffer::Create(fbSpec);

		mCurrentScene = CreateRef<Scene>();

#if 0
		auto camera1 = mCurrentScene->CreateEntity("Camera A");
		camera1.addComponent<CameraComponent>();

		auto camera2 = mCurrentScene->CreateEntity("Camera B");
		auto& cc = camera2.addComponent<CameraComponent>();
		cc.primary = false;

		class CameraController : public ScriptableEntity
		{
		public:
			void onCreate() override
			{
			}

			void onDestroy() override
			{
			}

			void onUpdate(Timestep ts) override
			{
				auto& translation = getComponent<TransformComponent>().translation;
				float speed = 5.0f;

				if (!getComponent<CameraComponent>().primary) return;

				if (Input::IsKeyPressed(LAB_KEY_A))
					translation.x -= speed * ts;
				if (Input::IsKeyPressed(LAB_KEY_D))
					translation.x += speed * ts;
				if (Input::IsKeyPressed(LAB_KEY_W))
					translation.y += speed * ts;
				if (Input::IsKeyPressed(LAB_KEY_S))
					translation.y -= speed * ts;
			}
		};

		camera1.addComponent<NativeScriptComponent>().bind<CameraController>();
		camera2.addComponent<NativeScriptComponent>().bind<CameraController>();
#endif
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
			mCameraController.onResize(mViewportSize.x, mViewportSize.y);

			mCurrentScene->onViewportResize((uint32_t)mViewportSize.x, (uint32_t)mViewportSize.y);
		}

		if (mViewportFocused)
			mCameraController.onUpdate(ts);

		Renderer2D::ResetStats();

		mFramebuffer->bind();
		RenderCommand::SetClearColor({ 0.125f, 0.0625f, 0.25f, 1.0f });
		RenderCommand::Clear();

		mCurrentScene->onUpdate(ts);

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
				if (ImGui::MenuItem("Open...", "Ctrl+O"))
					OpenScene();
				if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
					SaveSceneAs();

				if (ImGui::MenuItem("Exit")) Application::Get().Close();
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		mScenePanel.onImGuiRender();

		ImGui::Begin("Stats");

		auto stats = Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.drawCalls);
		ImGui::Text("Quads: %d", stats.quadCount);
		ImGui::Text("Vertices: %d", stats.getTotalVertexCount());
		ImGui::Text("Indices: %d", stats.getTotalIndexCount());

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");

		mViewportFocused = ImGui::IsWindowFocused();
		mViewportHovered = ImGui::IsWindowHovered();
		Application::Get().getImGuiLayer()->blockEvents(!mViewportFocused || !mViewportHovered);

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		mViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		uint32_t textureID = mFramebuffer->getColorAttachmentRendererID();
		ImGui::Image((void*)textureID, ImVec2{ mViewportSize.x, mViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::End();

	}

	void EditorLayer::onEvent(Event& e)
	{
		mCameraController.onEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.dispatch<KeyPressedEvent>(LAB_BIND_EVENT_FUNC(EditorLayer::OnKeyPressed));
		dispatcher.dispatch<MouseButtonPressedEvent>(LAB_BIND_EVENT_FUNC(EditorLayer::OnMousePressedEvent));
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

			break;
		}
		}
		return false;
	}

	bool EditorLayer::OnMousePressedEvent(MouseButtonPressedEvent& e)
	{
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
		std::string filepath = FileDialogs::OpenFile("Labyrinth Scene (*.laby)\0*.laby\0" "Labyrinth Entity(*.lbent)\0* .lbent\0");
		if (!filepath.empty())
		{
			mCurrentScene = CreateRef<Scene>();
			mCurrentScene->onViewportResize((uint32_t)mViewportSize.x, (uint32_t)mViewportSize.y);
			mScenePanel.setContext(mCurrentScene);

			SceneSerialiser serialiser(mCurrentScene);
			serialiser.deserialise(filepath);
		}
	}

	void EditorLayer::SaveSceneAs()
	{
		std::string filepath = FileDialogs::SaveFile("Labyrinth Scene (*.laby)\0*.laby\0" "Labyrinth Entity(*.lbent)\0* .lbent\0");
		if (!filepath.empty())
		{
			SceneSerialiser serialiser(mCurrentScene);
			serialiser.serialise(filepath);
		}
	}
}