#pragma once

#include <glm/ext/vector_float3.hpp>

struct ImVec2;

namespace Labyrinth {

	class EditorUI
	{
	public:
		static void Vec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);

		static void GridControl(const ImVec2& region, i32 width, i32 height, std::function<void(i32, i32, const ImVec2&)> lambda);
	};
}
