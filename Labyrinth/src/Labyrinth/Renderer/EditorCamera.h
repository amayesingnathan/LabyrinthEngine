#pragma once

#include "Camera.h"
#include "Labyrinth/Core/Timestep.h"
#include "Labyrinth/Events/Event.h"

namespace Laby {

	class EditorCamera : public Camera
	{
	public:
		EditorCamera() = default;
		EditorCamera(f32 fov, f32 aspectRatio, f32 nearClip, f32 farClip);

		void onUpdate(Timestep ts);
		void onEvent(Event& e);

		glm::mat4 getViewProjection() const override { return mProjection * mView; }

		inline f32 getDistance() const { return mDistance; }
		inline void setDistance(f32 distance) { mDistance = distance; }

		inline void setViewportSize(f32 width, f32 height) { mViewportWidth = width; mViewportHeight = height; UpdateProjection(); }

		glm::vec3 getUpDirection() const;
		glm::vec3 getRightDirection() const;
		glm::vec3 getForwardDirection() const;
		const glm::vec3& getPosition() const { return mPosition; }
		glm::quat getOrientation() const;

		f32 getPitch() const { return mPitch; }
		f32 getYaw() const { return mYaw; }

		void resetPosition();
		void resetAngle();

	private:
		void UpdateProjection();
		void UpdateView();

		bool OnMouseScroll(MouseScrolledEvent& e);

		void MousePan(const glm::vec2& delta);
		void MouseRotate(const glm::vec2& delta);
		void MouseZoom(f32 delta);

		glm::vec3 CalculatePosition() const;

		glm::vec2 PanSpeed() const;
		f32 RotationSpeed() const;
		f32 ZoomSpeed() const;

	private:
		glm::mat4 mView;

		f32 mFOV = 45.0f, mAspectRatio = 1.778f, mNearClip = 0.1f, mFarClip = 1000.0f;

		glm::vec3 mPosition = { 0.0f, 0.0f, 0.0f };
		glm::vec3 mFocalPoint = { 0.0f, 0.0f, 0.0f };

		glm::vec2 mInitialMousePosition = { 0.0f, 0.0f };

		f32 mDistance = 10.0f;
		f32 mPitch = 0.0f, mYaw = 0.0f;

		f32 mViewportWidth = 1280, mViewportHeight = 720;
	};

}