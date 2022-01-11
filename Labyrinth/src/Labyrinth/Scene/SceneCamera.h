#pragma once

#include "Labyrinth/Renderer/Camera.h"

#include "Labyrinth/Maths/Constant.h"

namespace Labyrinth {

	class SceneCamera : public Camera
	{
	public:
		enum class ProjectionType { Perspective = 0, Orthographic = 1 };

	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		void setOrthographic(float size, float nearClip, float farClip);
		void setPerspective(float verticalFOV, float nearClip, float farClip);

		void setViewportSize(uint32_t width, uint32_t height);

		float getPerspectiveVerticalFOV() const { return mPerspectiveFOV; }
		void setPerspectiveVerticalFOV(float verticalFov) { mPerspectiveFOV = verticalFov; RecalculateProjection(); }
		float getPerspectiveNearClip() const { return mPerspectiveNear; }
		void setPerspectiveNearClip(float nearClip) { mPerspectiveNear = nearClip; RecalculateProjection(); }
		float getPerspectiveFarClip() const { return mPerspectiveFar; }
		void setPerspectiveFarClip(float farClip) { mPerspectiveFar = farClip; RecalculateProjection(); }

		float getOrthographicSize() const { return mOrthographicSize; }
		void setOrthographicSize(float size) { mOrthographicSize = size; RecalculateProjection(); }
		float getOrthographicNearClip() const { return mOrthographicNear; }
		void setOrthographicNearClip(float nearClip) { mOrthographicNear = nearClip; RecalculateProjection(); }
		float getOrthographicFarClip() const { return mOrthographicFar; }
		void setOrthographicFarClip(float farClip) { mOrthographicFar = farClip; RecalculateProjection(); }

		ProjectionType getProjectionType() const { return mProjectionType; }
		void setProjectionType(ProjectionType type) { mProjectionType = type; RecalculateProjection(); }

	private:
		void RecalculateProjection();

	private:
		ProjectionType mProjectionType = ProjectionType::Orthographic;

		float mPerspectiveFOV = glm::radians(LAB_ANG_45);
		float mPerspectiveNear = 0.01f, mPerspectiveFar = 1000.0f;

		float mOrthographicSize = 10.0f;
		float mOrthographicNear = -1.0f, mOrthographicFar = 1.0f;

		float mAspectRatio = 0.0f;
	};
}