#pragma once

#include <Labyrinth/Core/System/Base.h>

struct ImVec2;
struct ImVec3;
struct ImVec4;

typedef int ImGuiStyleVar;
typedef int ImGuiTreeNodeFlags;

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

		static glm::vec2 AvailableRegion();

		static f32 FontSize();
		static glm::vec2 FramePadding();

		static f32 LineHeight();

		static f32 WindowWidth();
		static f32 WindowHeight();

		static void PushItemWidth(f32 width);
		static void PopItemWidth();

		static void PushStyle(ImGuiStyleVar flags, f32 var);
		static void PushStyle(ImGuiStyleVar flags, const glm::vec2& var);
		static void PopStyle(i32 count = 1);
	};
}