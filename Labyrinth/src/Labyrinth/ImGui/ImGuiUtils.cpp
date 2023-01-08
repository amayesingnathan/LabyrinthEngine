#include "Lpch.h"
#include "ImGuiUtils.h"

#include <imgui.h>

namespace Laby {

	const ImVec2& ImGuiUtils::FromGLM(const glm::vec2& vec)
	{
		return *(ImVec2*)&vec;
	}

	const ImVec3& ImGuiUtils::FromGLM(const glm::vec3& vec)
	{
		return *(ImVec3*)&vec;
	}

	const ImVec4& ImGuiUtils::FromGLM(const glm::vec4& vec)
	{
		return *(ImVec4*)&vec;
	}
}