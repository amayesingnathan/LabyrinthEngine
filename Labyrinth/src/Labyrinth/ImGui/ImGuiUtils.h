#pragma once

#include <Labyrinth/Core/System/Base.h>

struct ImVec2;
struct ImVec3;
struct ImVec4;

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

		static f32 WindowWidth();
		static f32 WindowHeight();

		static void PushItemWidth(f32 width);
		static void PopItemWidth();
	};
}