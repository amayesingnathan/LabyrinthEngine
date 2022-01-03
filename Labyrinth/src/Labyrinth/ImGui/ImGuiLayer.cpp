#include "Lpch.h"
#include "ImGuiLayer.h"

#include "Labyrinth/Core/Application.h"

#include "imgui.h"
#include "Labyrinth/Platforms/OpenGL/imgui_impl_opengl3.h"
#include "Labyrinth/Platforms/SDL/imgui_impl_sdl.h"
#include "Labyrinth/Platforms/SDL/imgui_impl_sdl.cpp"

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

		// Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
	// GL ES 2.0 + GLSL 100
		const char* glsl_version = "#version 100";
#elif defined(__APPLE__)
	// GL 3.2 Core + GLSL 150
		const char* glsl_version = "#version 150";
#else
	// GL 3.0 + GLSL 130
		const char* glsl_version = "#version 130";
#endif

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

		ImGui::StyleColorsDark();
		// Setup Platform/Renderer backends
		ImGui_ImplSDL2_InitForOpenGL(SDL_GL_GetCurrentWindow(), SDL_GL_GetCurrentContext());
		ImGui_ImplOpenGL3_Init(glsl_version);
	}

	void ImGuiLayer::onDetach()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::onUpdate()
	{
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2(app.getWindow().getWidth(), app.getWindow().getHeight());

		int time = SDL_GetTicks();
		io.DeltaTime = mTime > 0 ? (time - mTime) : (1.f / 60.f);
		mTime = time;

		static bool show = true;
		ImGui::ShowDemoWindow(&show);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void ImGuiLayer::onEvent(Event& e)
	{
		//if (mBlockEvents)
		//{
		//	ImGuiIO& io = ImGui::GetIO();
		//	e.handled |= e.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
		//	e.handled |= e.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
		//}
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

	bool ImGuiLayer::OnMouseButtonPressedEvent(MouseButtonPressedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGui_ImplSDL2_Data* bd = ImGui_ImplSDL2_GetBackendData();

		io.MouseDown[e.GetMouseButton()] = true;
		
		if (e.GetMouseButton() == SDL_BUTTON_LEFT) { bd->MousePressed[0] = true; }
		if (e.GetMouseButton() == SDL_BUTTON_RIGHT) { bd->MousePressed[1] = true; }
		if (e.GetMouseButton() == SDL_BUTTON_MIDDLE) { bd->MousePressed[2] = true; }


		return false;
	}

	bool ImGuiLayer::OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGui_ImplSDL2_Data* bd = ImGui_ImplSDL2_GetBackendData();

		io.MouseDown[e.GetMouseButton()] = false;

		if (e.GetMouseButton() == SDL_BUTTON_LEFT) { bd->MousePressed[0] = false; }
		if (e.GetMouseButton() == SDL_BUTTON_RIGHT) { bd->MousePressed[1] = false; }
		if (e.GetMouseButton() == SDL_BUTTON_MIDDLE) { bd->MousePressed[2] = false; }
		return false;
	}

	bool ImGuiLayer::OnMouseMovedEvent(MouseMovedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2(e.GetX(), e.GetY());

		return false;
	}

	bool ImGuiLayer::OnMouseScrolledEvent(MouseScrolledEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheel += e.GetYOffset();
		io.MouseWheelH += e.GetXOffset();

		return false;
	}

	bool ImGuiLayer::OnKeyPressedEvent(KeyPressedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[e.GetKeyCode()] = true;
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
		io.KeysDown[e.GetKeyCode()] = false;
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
		io.AddInputCharactersUTF8(e.GetText().c_str());
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