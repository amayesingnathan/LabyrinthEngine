#pragma once

#include <glm/glm.hpp>

namespace Laby {

	class Camera
	{
	public:
		Camera() = default;
		Camera(const glm::mat4& projection)
			: mProjection(projection) {}

		glm::mat4 getViewProjection() const { return mProjection * mView; }

	protected:
		glm::mat4 mView = glm::mat4(1.0f);
		glm::mat4 mProjection = glm::mat4(1.0f);
	};
}