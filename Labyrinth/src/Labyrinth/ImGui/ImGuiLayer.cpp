#include "Lpch.h"
#include "ImGuiLayer.h"

#include "Labyrinth/Core/Application.h"
#include "Labyrinth/Core/Input.h"
#include "Labyrinth/Core/MouseButtonCodes.h"

#include "imgui.h"

#include "backends/imgui_impl_sdl.h"
#include "backends/imgui_impl_opengl3.h"

#include "SDL.h"
#include "Glad/glad.h"
#include "SDL_opengl.h"

#if SDL_VERSION_ATLEAST(2,0,4) && !defined(__EMSCRIPTEN__) && !defined(__ANDROID__) && !(defined(__APPLE__) && TARGET_OS_IOS)
#define SDL_HAS_CAPTURE_AND_GLOBAL_MOUSE    1
#else
#define SDL_HAS_CAPTURE_AND_GLOBAL_MOUSE    0
#endif
#define SDL_HAS_MOUSE_FOCUS_CLICKTHROUGH    SDL_VERSION_ATLEAST(2,0,5)
#define SDL_HAS_WINDOW_ALPHA                SDL_VERSION_ATLEAST(2,0,5)
#define SDL_HAS_ALWAYS_ON_TOP               SDL_VERSION_ATLEAST(2,0,5)
#define SDL_HAS_USABLE_DISPLAY_BOUNDS       SDL_VERSION_ATLEAST(2,0,5)
#define SDL_HAS_PER_MONITOR_DPI             SDL_VERSION_ATLEAST(2,0,4)
#define SDL_HAS_VULKAN                      SDL_VERSION_ATLEAST(2,0,6)
#if !SDL_HAS_VULKAN
static const Uint32 SDL_WINDOW_VULKAN = 0x10000000;
#endif

// SDL Data
struct ImGui_ImplSDL2_Data
{
	SDL_Window* Window;
	Uint64      Time;
	bool        MousePressed[3];
	SDL_Cursor* MouseCursors[ImGuiMouseCursor_COUNT];
	char* ClipboardTextData;
	bool        MouseCanUseGlobalState;
	bool        UseVulkan;

	ImGui_ImplSDL2_Data() { memset(this, 0, sizeof(*this)); }
};

namespace Labyrinth {

	static ImGui_ImplSDL2_Data* ImGui_ImplSDL2_GetBackendData()
	{
		return ImGui::GetCurrentContext() ? (ImGui_ImplSDL2_Data*)ImGui::GetIO().BackendPlatformUserData : NULL;
	}


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
		ImGui_ImplSDL2_Data* bd = ImGui_ImplSDL2_GetBackendData();

		ImVec2 mouse_pos_prev = io.MousePos;
		io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
		io.MouseHoveredViewport = 0;

		// Get mouse positions
		float mouse_x_local = Input::GetMouseX();
		float mouse_y_local = Input::GetMouseY();;


		///////////////////////////////////////////////////////////////////
		///		SDL CODE OUGHT TO BE ABSTRACTED INTO INPUT CLASSES		///
		///////////////////////////////////////////////////////////////////

		// Obtain focused and hovered window. We forward mouse input when focused or when hovered (and no other window is capturing)
#if SDL_HAS_CAPTURE_AND_GLOBAL_MOUSE
		SDL_Window* focused_window = SDL_GetKeyboardFocus();
		SDL_Window* hovered_window = SDL_HAS_MOUSE_FOCUS_CLICKTHROUGH ? SDL_GetMouseFocus() : NULL; // This is better but is only reliably useful with SDL 2.0.5+ and SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH.
		SDL_Window* mouse_window = NULL;
		if (hovered_window && (bd->Window == hovered_window || ImGui::FindViewportByPlatformHandle((void*)hovered_window)))
			mouse_window = hovered_window;
		else if (focused_window && (bd->Window == focused_window || ImGui::FindViewportByPlatformHandle((void*)focused_window)))
			mouse_window = focused_window;

		// SDL_CaptureMouse() let the OS know e.g. that our imgui drag outside the SDL window boundaries shouldn't e.g. trigger other operations outside
		SDL_CaptureMouse(ImGui::IsAnyMouseDown() ? SDL_TRUE : SDL_FALSE);
#else
	// SDL 2.0.3 and non-windowed systems: single-viewport only
		SDL_Window* mouse_window = (SDL_GetWindowFlags(bd->Window) & SDL_WINDOW_INPUT_FOCUS) ? bd->Window : NULL;
#endif

		if (mouse_window == NULL)
			return false;

		// Set OS mouse position from Dear ImGui if requested (rarely used, only when ImGuiConfigFlags_NavEnableSetMousePos is enabled by user)
		if (io.WantSetMousePos)
		{
#if SDL_HAS_CAPTURE_AND_GLOBAL_MOUSE
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
				SDL_WarpMouseGlobal((int)mouse_pos_prev.x, (int)mouse_pos_prev.y);
			else
#endif
				SDL_WarpMouseInWindow(bd->Window, (int)mouse_pos_prev.x, (int)mouse_pos_prev.y);
		}

		// Set Dear ImGui mouse position from OS position + get buttons. (this is the common behavior)
		if (bd->MouseCanUseGlobalState)
		{
			float mouse_x_global = Input::GetGlobalMouseX();
			float mouse_y_global = Input::GetGlobalMouseY();
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				// Multi-viewport mode: mouse position in OS absolute coordinates (io.MousePos is (0,0) when the mouse is on the upper-left of the primary monitor)
				io.MousePos = ImVec2(mouse_x_global, mouse_y_global);
			}
			else
			{
				// Single-viewport mode: mouse position in client window coordinates (io.MousePos is (0,0) when the mouse is on the upper-left corner of the app window)
				// Unlike local position obtained earlier this will be valid when straying out of bounds.
				int window_x, window_y;
				SDL_GetWindowPosition(mouse_window, &window_x, &window_y);
				io.MousePos = ImVec2((mouse_x_global - (float)window_x), (mouse_y_global - (float)window_y));
			}
		}
		else
		{
			io.MousePos = ImVec2(mouse_x_local, mouse_y_local);
		}

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