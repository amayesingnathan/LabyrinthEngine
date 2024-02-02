#pragma once

#include "Camera.h"
#include "Labyrinth/Core/Timestep.h"
#include "Labyrinth/Events/Event.h"

namespace Laby {

	class EditorCamera : public Camera, public IEventListener
	{
	public:
		EditorCamera() = default;
		EditorCamera(f32 fov, f32 aspectRatio, f32 nearClip, f32 farClip);

		void OnUpdate(Timestep ts);

		void OnEvent(Event& e) override;
		LISTENING_EVENTS(MouseScrolled)

		const glm::mat4& GetViewMatrix() const { return mView; }
		glm::mat4 GetViewProjection() const override { return mProjection * mView; }

		inline f32 GetDistance() const { return mDistance; }
		inline void SetDistance(f32 distance) { mDistance = distance; }

		inline void SetViewportSize(f32 width, f32 height) { mViewportWidth = width; mViewportHeight = height; UpdateProjection(); }

		glm::vec3 GetUpDirection() const;
		glm::vec3 GetRightDirection() const;
		glm::vec3 GetForwardDirection() const;
		const glm::vec3& GetPosition() const { return mPosition; }
		glm::quat GetOrientation() const;

		f32 GetPitch() const { return mPitch; }
		f32 GetYaw() const { return mYaw; }

		void ResetPosition();
		void ResetAngle();

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