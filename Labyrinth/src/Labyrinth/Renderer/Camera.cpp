#include "Lpch.h"
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Labyrinth {

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top, float zoom)
		: mProjectionMatrix(glm::ortho(left * zoom, right * zoom, bottom * zoom, top * zoom, -1.0f, 1.0f)),
		  mViewMatrix(1.0f), mZoom(zoom)
	{
		mViewProjectionMatrix = mProjectionMatrix * mViewMatrix;
	}

	void OrthographicCamera::setProjection(float left, float right, float bottom, float top)
	{
		mProjectionMatrix = glm::ortho((left * mZoom), (right * mZoom), (bottom * mZoom), (top * mZoom), -1.0f, 1.0f);
		mViewProjectionMatrix = mProjectionMatrix * mViewMatrix;
	}

	void OrthographicCamera::recalculateViewMatrix()
	{		 
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), mPosition) * 
			glm::rotate(glm::mat4(1.0f), glm::radians(mRotation), glm::vec3(0, 0, 1));

		mViewMatrix = glm::inverse(transform);
		mViewProjectionMatrix = mProjectionMatrix * mViewMatrix;
	}

}