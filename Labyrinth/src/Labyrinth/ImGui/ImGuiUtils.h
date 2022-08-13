#pragma once
#include <glm/ext/vector_float3.hpp>

namespace Labyrinth {

	class EditorUI
	{
	public:
		static void Vec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);
	};
}
