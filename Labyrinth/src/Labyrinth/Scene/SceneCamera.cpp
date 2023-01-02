#include "Lpch.h"
#include "SceneCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Laby {

	SceneCamera::SceneCamera()
	{
		RecalculateProjection();
	}

	void SceneCamera::setPerspective(f32 verticalFOV, f32 nearClip, f32 farClip)
	{
		mProjectionType = ProjectionType::Perspective;
		mPerspectiveFOV = verticalFOV;
		mPerspectiveNear = nearClip;
		mPerspectiveFar = farClip;
		RecalculateProjection();
	}

	void SceneCamera::setOrthographic(f32 size, f32 nearClip, f32 farClip)
	{
		mProjectionType = ProjectionType::Orthographic;
		mOrthographicSize = size;
		mOrthographicNear = nearClip;
		mOrthographicFar = farClip;
		RecalculateProjection();
	}

	void SceneCamera::setViewportSize(u32 width, u32 height)
	{
		LAB_CORE_ASSERT(width > 0 && height > 0);
		mAspectRatio = (f32)width / (f32)height;
		RecalculateProjection();
	}

	void SceneCamera::RecalculateProjection()
	{
		if (mProjectionType == ProjectionType::Perspective)
		{
			mProjection = glm::perspective(mPerspectiveFOV, mAspectRatio, mPerspectiveNear, mPerspectiveFar);
		}
		else
		{
			f32 orthoLeft = -mOrthographicSize * mAspectRatio * 0.5f;
			f32 orthoRight = mOrthographicSize * mAspectRatio * 0.5f;
			f32 orthoBottom = -mOrthographicSize * 0.5f;
			f32 orthoTop = mOrthographicSize * 0.5f;

			mProjection = glm::ortho(orthoLeft, orthoRight,
				orthoBottom, orthoTop, mOrthographicNear, mOrthographicFar);
		}
	}
}