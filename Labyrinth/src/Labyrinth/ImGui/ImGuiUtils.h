#pragma once

#include <Labyrinth/Core/System/Base.h>

struct ImVec2;
struct ImVec3;
struct ImVec4;

namespace Laby {

	class ImGuiUtils
	{
	public:
		static const ImVec2& FromGLM(const glm::vec2& vec);
		static const ImVec3& FromGLM(const glm::vec3& vec);
		static const ImVec4& FromGLM(const glm::vec4& vec);
	};
}