#include "Lpch.h"
#include "ImGuiLayer.h"

#include "Labyrinth/Core/Application.h"

#include "imgui.h"

#include "backends/imgui_impl_sdl.h"
#include "backends/imgui_impl_opengl3.h"

#include "SDL.h"
#include "Glad/glad.h"
#include "SDL_opengl.h"

namespace Labyrinth {

	ImGuiLayer::ImGuiLayer() 
		: Layer("ImGuiLayer")
	{
	}

	void ImGuiLayer::onAttach()
	{
		LAB_PROFILE_FUNCTION();

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;
		
		io.Fonts->AddFontFromFileTTF("assets/fonts/CaviarDreams/Caviar_Dreams_Bold.ttf", 18.0f);
		io.Fonts->AddFontFromFileTTF("assets/fonts/CaviarDreams/CaviarDreams.ttf", 18.0f);

		// Setup Dear ImGui style
		SetDarkThemeColours();
		
		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		Application& app = Application::Get();
		SDL_Window* window = static_cast<SDL_Window*>(app.getWindow().getNativeWindow());
		SDL_GLContext context = static_cast<SDL_GLContext>(app.getWindow().getNativeContext());

		// Setup Platform/Renderer backends
		ImGui_ImplSDL2_InitForOpenGL(window, context);
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayer::onDetach()
	{
		LAB_PROFILE_FUNCTION();

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::onEvent(Event& e)
	{
		LAB_PROFILE_FUNCTION();

		if (mBlockEvents)
		{
			ImGuiIO& io = ImGui::GetIO();
			e.handled |= e.isInCategory(EventCategoryMouse) & io.WantCaptureMouse;
			e.handled |= e.isInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
		}

		EventDispatcher dispatcher(e);
		dispatcher.dispatch<MouseButtonPressedEvent>(LAB_BIND_EVENT_FUNC(ImGuiLayer::OnMouseButtonPressedEvent));
		dispatcher.dispatch<MouseButtonReleasedEvent>(LAB_BIND_EVENT_FUNC(ImGuiLayer::OnMouseButtonReleasedEvent));
		dispatcher.dispatch<MouseMovedEvent>(LAB_BIND_EVENT_FUNC(ImGuiLayer::OnMouseMovedEvent));
		dispatcher.dispatch<MouseScrolledEvent>(LAB_BIND_EVENT_FUNC(ImGuiLayer::OnMouseScrolledEvent));
		dispatcher.dispatch<KeyPressedEvent>(LAB_BIND_EVENT_FUNC(ImGuiLayer::OnKeyPressedEvent));
		dispatcher.dispatch<KeyReleasedEvent>(LAB_BIND_EVENT_FUNC(ImGuiLayer::OnKeyReleasedEvent));
		dispatcher.dispatch<KeyTypedEvent>(LAB_BIND_EVENT_FUNC(ImGuiLayer::OnKeyTypedEvent));
		dispatcher.dispatch<WindowResizeEvent>(LAB_BIND_EVENT_FUNC(ImGuiLayer::OnWindowResizeEvent));
		dispatcher.dispatch<WindowCloseEvent>(LAB_BIND_EVENT_FUNC(ImGuiLayer::OnWindowCloseEvent));

	}

	void ImGuiLayer::begin()
	{
		LAB_PROFILE_FUNCTION();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();	

	}

	void ImGuiLayer::end()
	{
		LAB_PROFILE_FUNCTION();

		ImGuiIO& io = ImGui::GetIO();

		Application& app = Application::Get();
		io.DisplaySize = ImVec2(static_cast<float>(app.getWindow().getWidth()), static_cast<float>(app.getWindow().getHeight()));

		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
			SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
		}
	}

	bool ImGuiLayer::OnMouseButtonPressedEvent(MouseButtonPressedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();

		io.MouseDown[e.getMouseButton()] = true;

		return false;
	}

	bool ImGuiLayer::OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();

		io.MouseDown[e.getMouseButton()] = false;

		return false;
	}

	bool ImGuiLayer::OnMouseMovedEvent(MouseMovedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2(e.getX(), e.getY());

		return false;
	}

	bool ImGuiLayer::OnMouseScrolledEvent(MouseScrolledEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheel += e.getYOffset();
		io.MouseWheelH += e.getXOffset();

		return false;
	}

	bool ImGuiLayer::OnKeyPressedEvent(KeyPressedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[e.getKeyCode()] = true;
		io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
		io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
		io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
#ifdef _WIN32
		io.KeySuper = false;
#else
		io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);
#endif

		return false;
	}

	bool ImGuiLayer::OnKeyReleasedEvent(KeyReleasedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[e.getKeyCode()] = false;
		io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
		io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
		io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
#ifdef _WIN32
		io.KeySuper = false;
#else
		io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);
#endif
		return false;
	}

	bool ImGuiLayer::OnKeyTypedEvent(KeyTypedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.AddInputCharactersUTF8(e.getText().c_str());
		return false;
	}

	bool ImGuiLayer::OnWindowResizeEvent(WindowResizeEvent& e)
	{
		ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle((void*)SDL_GetWindowFromID(e.getID()));
		viewport->PlatformRequestResize = true;

		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2(static_cast<float>(app.getWindow().getWidth()), static_cast<float>(app.getWindow().getHeight()));

		return false;
	}
	bool ImGuiLayer::OnWindowCloseEvent(WindowCloseEvent& e)
	{
		ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle((void*)SDL_GetWindowFromID(e.getID()));
		viewport->PlatformRequestClose = true;
		
		return false;
	}

	void ImGuiLayer::SetDarkThemeColours() 
	{
		auto& colours = ImGui::GetStyle().Colors;
		colours[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

		// Headers
		colours[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colours[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colours[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Buttons
		colours[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colours[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colours[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Frame BG
		colours[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colours[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colours[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Tabs
		colours[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colours[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
		colours[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
		colours[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colours[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

		// Title
		colours[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colours[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colours[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	}
}