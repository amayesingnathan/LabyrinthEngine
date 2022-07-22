#pragma once

#include "Labyrinth/Renderer/Camera.h"

#include "Labyrinth/Maths/Constant.h"

namespace Labyrinth {

	class LAB_API SceneCamera : public Camera
	{
	public:
		enum class ProjectionType { Perspective = 0, Orthographic = 1 };

	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		void setPerspective(f32 verticalFOV, f32 nearClip, f32 farClip);
		void setOrthographic(f32 size, f32 nearClip, f32 farClip);

		void setViewportSize(u32 width, u32 height);

		f32 getPerspectiveVerticalFOV() const { return mPerspectiveFOV; }
		void setPerspectiveVerticalFOV(f32 verticalFov) { mPerspectiveFOV = verticalFov; RecalculateProjection(); }
		f32 getPerspectiveNearClip() const { return mPerspectiveNear; }
		void setPerspectiveNearClip(f32 nearClip) { mPerspectiveNear = nearClip; RecalculateProjection(); }
		f32 getPerspectiveFarClip() const { return mPerspectiveFar; }
		void setPerspectiveFarClip(f32 farClip) { mPerspectiveFar = farClip; RecalculateProjection(); }

		f32 getOrthographicSize() const { return mOrthographicSize; }
		void setOrthographicSize(f32 size) { mOrthographicSize = size; RecalculateProjection(); }
		f32 getOrthographicNearClip() const { return mOrthographicNear; }
		void setOrthographicNearClip(f32 nearClip) { mOrthographicNear = nearClip; RecalculateProjection(); }
		f32 getOrthographicFarClip() const { return mOrthographicFar; }
		void setOrthographicFarClip(f32 farClip) { mOrthographicFar = farClip; RecalculateProjection(); }

		ProjectionType getProjectionType() const { return mProjectionType; }
		void setProjectionType(ProjectionType type) { mProjectionType = type; RecalculateProjection(); }

	private:
		void RecalculateProjection();

	private:
		ProjectionType mProjectionType = ProjectionType::Orthographic;

		f32 mPerspectiveFOV = glm::radians(LAB_ANG_45);
		f32 mPerspectiveNear = 0.01f, mPerspectiveFar = 1000.0f;

		f32 mOrthographicSize = 10.0f;
		f32 mOrthographicNear = -1.0f, mOrthographicFar = 1.0f;

		f32 mAspectRatio = 0.0f;
	};
}