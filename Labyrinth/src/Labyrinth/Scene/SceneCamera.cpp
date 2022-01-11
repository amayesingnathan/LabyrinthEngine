#include "Lpch.h"
#include "SceneCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Labyrinth {

	SceneCamera::SceneCamera()
	{
		RecalculateProjection();
	}

	void SceneCamera::setOrthographic(float size, float nearClip, float farClip)
	{
		mProjectionType = ProjectionType::Orthographic;
		mOrthographicSize = size;
		mOrthographicNear = nearClip;
		mOrthographicFar = farClip;
		RecalculateProjection();
	}

	void SceneCamera::setPerspective(float verticalFOV, float nearClip, float farClip)
	{
		mProjectionType = ProjectionType::Perspective;
		mPerspectiveFOV = verticalFOV;
		mPerspectiveNear = nearClip;
		mPerspectiveFar = farClip;
		RecalculateProjection();
	}

	void SceneCamera::setViewportSize(uint32_t width, uint32_t height)
	{
		mAspectRatio = (float)width / (float)height;
		RecalculateProjection();
	}

	void SceneCamera::RecalculateProjection()
	{
		if (mProjectionType == ProjectionType::Perspective)
		{
			mProjectionMatrix = glm::perspective(mPerspectiveFOV, mAspectRatio, mPerspectiveNear, mPerspectiveFar);
		}
		else
		{
			float orthoLeft = -mOrthographicSize * mAspectRatio * 0.5f;
			float orthoRight = mOrthographicSize * mAspectRatio * 0.5f;
			float orthoBottom = -mOrthographicSize * 0.5f;
			float orthoTop = mOrthographicSize * 0.5f;

			mProjectionMatrix = glm::ortho(orthoLeft, orthoRight,
				orthoBottom, orthoTop, mOrthographicNear, mOrthographicFar);
		}
	}
}