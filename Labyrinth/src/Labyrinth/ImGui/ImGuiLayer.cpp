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

	ImGuiLayer::~ImGuiLayer()
	{
	}

	void ImGuiLayer::onAttach()
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;
		// 
		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();
		
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
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::onEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<MouseButtonPressedEvent>(LAB_BIND_EVENT_FUNC(ImGuiLayer::OnMouseButtonPressedEvent));
		dispatcher.dispatch<MouseButtonReleasedEvent>(LAB_BIND_EVENT_FUNC(ImGuiLayer::OnMouseButtonReleasedEvent));
		dispatcher.dispatch<MouseMovedEvent>(LAB_BIND_EVENT_FUNC(ImGuiLayer::OnMouseMovedEvent));
		dispatcher.dispatch<MouseScrolledEvent>(LAB_BIND_EVENT_FUNC(ImGuiLayer::OnMouseScrolledEvent));
		dispatcher.dispatch<KeyPressedEvent>(LAB_BIND_EVENT_FUNC(ImGuiLayer::OnKeyPressedEvent));
		dispatcher.dispatch<KeyReleasedEvent>(LAB_BIND_EVENT_FUNC(ImGuiLayer::OnKeyReleasedEvent));
		dispatcher.dispatch<KeyTypedEvent>(LAB_BIND_EVENT_FUNC(ImGuiLayer::OnKeyTypedEvent));
		dispatcher.dispatch<WindowResizeEvent>(LAB_BIND_EVENT_FUNC(ImGuiLayer::OnWindowResizeEvent));
	}

	void ImGuiLayer::begin()
	{
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();	

		//ImGuiIO& io = ImGui::GetIO();
		//Application& app = Application::Get();
		//io.DisplaySize = ImVec2(app.getWindow().getWidth(), app.getWindow().getHeight());

		//int time = SDL_GetTicks();
		//io.DeltaTime = mTime > 0 ? (time - mTime) : (1.f / 60.f);
		//mTime = time;

		//static bool show = true;
		//ImGui::ShowDemoWindow(&show);

		//ImGui::Render();
		//ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void ImGuiLayer::end()
	{
		ImGuiIO& io = ImGui::GetIO();

		Application& app = Application::Get();
		io.DisplaySize = ImVec2(app.getWindow().getWidth(), app.getWindow().getHeight());

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

	void ImGuiLayer::onImGuiRender()
	{
		static bool show = true;
		ImGui::ShowDemoWindow(&show);
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
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2(app.getWindow().getWidth(), app.getWindow().getHeight());

		return false;
	}
}