#pragma once

#include <Labyrinth/Core/System/Base.h>

namespace Laby {

	class Camera
	{
	public:
		Camera() = default;
		Camera(const glm::mat4& projection)
			: mProjection(projection) {}

		glm::mat4 getProjection() const { return mProjection; }

		virtual glm::mat4 getViewProjection() const { LAB_CORE_ASSERT(false, "Need to implement view matrix to use this function!"); return glm::mat4{ 1.0f }; }

	protected:
		glm::mat4 mProjection = glm::mat4(1.0f);
	};
}