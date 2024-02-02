#pragma once
#pragma once

#include <Labyrinth/Core/System/Base.h>
#include <Labyrinth/Renderer/Camera.h>

namespace Laby {

	class SceneCamera : public Camera
	{
	public:
		enum class ProjectionType { Perspective = 0, Orthographic = 1 };

	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		void SetPerspective(f32 verticalFOV, f32 nearClip, f32 farClip);
		void SetOrthographic(f32 size, f32 nearClip, f32 farClip);

		void SetViewportSize(u32 width, u32 height);

		f32 GetPerspectiveVerticalFOV() const { return mPerspectiveFOV; }
		void SetPerspectiveVerticalFOV(f32 verticalFov) { mPerspectiveFOV = verticalFov; RecalculateProjection(); }
		f32 GetPerspectiveNearClip() const { return mPerspectiveNear; }
		void SetPerspectiveNearClip(f32 nearClip) { mPerspectiveNear = nearClip; RecalculateProjection(); }
		f32 GetPerspectiveFarClip() const { return mPerspectiveFar; }
		void SetPerspectiveFarClip(f32 farClip) { mPerspectiveFar = farClip; RecalculateProjection(); }

		f32 GetOrthographicSize() const { return mOrthographicSize; }
		void SetOrthographicSize(f32 size) { mOrthographicSize = size; RecalculateProjection(); }
		f32 GetOrthographicNearClip() const { return mOrthographicNear; }
		void SetOrthographicNearClip(f32 nearClip) { mOrthographicNear = nearClip; RecalculateProjection(); }
		f32 GetOrthographicFarClip() const { return mOrthographicFar; }
		void SetOrthographicFarClip(f32 farClip) { mOrthographicFar = farClip; RecalculateProjection(); }

		const ProjectionType& GetProjectionType() const { return mProjectionType; }
		void SetProjectionType(ProjectionType type) { mProjectionType = type; RecalculateProjection(); }

	private:
		void RecalculateProjection();

	private:
		ProjectionType mProjectionType = ProjectionType::Perspective;

		f32 mPerspectiveFOV = Angle::Rad45f;
		f32 mPerspectiveNear = 0.01f, mPerspectiveFar = 1000.0f;

		f32 mOrthographicSize = 10.0f;
		f32 mOrthographicNear = -1.0f, mOrthographicFar = 1.0f;

		f32 mAspectRatio = 0.0f;
	};
}