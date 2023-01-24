#pragma once

#include <Labyrinth/IO/MouseCodes.h>

struct ImVec2;
struct ImVec3;
struct ImVec4;

typedef int ImGuiStyleVar;
typedef int ImGuiTreeNodeFlags;
typedef int ImGuiCol;

namespace Laby {

	class ImGuiUtils
	{
	public:
		static const glm::vec2& ToGLM(const ImVec2& vec);
		static const glm::vec3& ToGLM(const ImVec3& vec);
		static const glm::vec4& ToGLM(const ImVec4& vec);

		static const ImVec2& FromGLM(const glm::vec2& vec);
		static const ImVec3& FromGLM(const glm::vec3& vec);
		static const ImVec4& FromGLM(const glm::vec4& vec);

		static bool IsMouseDown(MouseCode button);
		static bool IsMouseReleased(MouseCode button);

		static glm::vec2 CursorPos();
		static void SetCursorPos(const glm::vec2& pos);
		static void SetCursorPosX(f32 pos);
		static void SetCursorPosY(f32 pos);

		static void SetButtonTransparent();
		static void ResetButtonTransparency();

		static glm::vec2 AvailableRegion();

		static f32 FontSize();
		static glm::vec2 FramePadding();
		static f32 FrameHeightWithSpacing();

		static f32 LineHeight();

		static f32 WindowWidth();
		static f32 WindowHeight();

		static void PushItemWidth(f32 width);
		static void PopItemWidth();

		static void PushID(std::string_view strID);
		static void PopID();

		static void PushStyle(ImGuiStyleVar flags, f32 var);
		static void PushStyle(ImGuiStyleVar flags, const glm::vec2& var);
		static void PopStyle(i32 count = 1);

		static void PushStyleColour(ImGuiCol flags, const glm::vec4& var);
		static void PopStyleColour();
	};
}