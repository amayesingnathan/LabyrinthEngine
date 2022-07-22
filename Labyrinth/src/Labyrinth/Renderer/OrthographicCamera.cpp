#include "Lpch.h"
#include "Labyrinth/Renderer/OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Labyrinth {

	OrthographicCamera::OrthographicCamera(f32 left, f32 right, f32 bottom, f32 top)
		: mProjectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), mViewMatrix(1.0f)
	{
		LAB_PROFILE_FUNCTION();

		mViewProjectionMatrix = mProjectionMatrix * mViewMatrix;
	}

	void OrthographicCamera::setProjection(f32 left, f32 right, f32 bottom, f32 top)
	{
		LAB_PROFILE_FUNCTION();

		mProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		mViewProjectionMatrix = mProjectionMatrix * mViewMatrix;
	}

	void OrthographicCamera::RecalculateViewMatrix()
	{
		LAB_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), mPosition) *
			glm::rotate(glm::mat4(1.0f), glm::radians(mRotation), glm::vec3(0, 0, 1));

		mViewMatrix = glm::inverse(transform);
		mViewProjectionMatrix = mProjectionMatrix * mViewMatrix;
	}

}