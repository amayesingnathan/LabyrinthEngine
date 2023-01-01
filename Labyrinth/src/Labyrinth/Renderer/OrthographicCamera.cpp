#include "Lpch.h"
#include "OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Laby {

	OrthographicCamera::OrthographicCamera(f32 left, f32 right, f32 bottom, f32 top)
		: Camera(glm::ortho(left, right, bottom, top, -1.0f, 1.0f))
	{
	}

	void OrthographicCamera::RecalculateViewMatrix()
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), mPosition) *
			glm::rotate(glm::mat4(1.0f), glm::radians(mRotation), glm::vec3(0, 0, 1));

		mView = glm::inverse(transform);
	}

}