#include "Lpch.h"
#include "EditorCamera.h"

#include "Labyrinth/IO/Input.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Laby {

	EditorCamera::EditorCamera(f32 fov, f32 aspectRatio, f32 nearClip, f32 farClip)
		: mFOV(fov), mAspectRatio(aspectRatio), mNearClip(nearClip), mFarClip(farClip), Camera(glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip))
	{
		UpdateView();
	}

	void EditorCamera::resetPosition()
	{
		mFocalPoint = glm::vec3(0.f);
		mDistance = 10.f;
		UpdateView();
	}

	void EditorCamera::resetAngle()
	{
		mPitch = 0.0f;
		mYaw = 0.0f;
		UpdateView();
	}

	void EditorCamera::UpdateProjection()
	{
		mAspectRatio = mViewportWidth / mViewportHeight;
		mProjection = glm::perspective(glm::radians(mFOV), mAspectRatio, mNearClip, mFarClip);
	}

	void EditorCamera::UpdateView()
	{
		// mYaw = mPitch = 0.0f; // Lock the camera's rotation
		mPosition = CalculatePosition();

		glm::quat orientation = getOrientation();
		mView = glm::translate(glm::mat4(1.0f), mPosition) * glm::toMat4(orientation);
		mView = glm::inverse(mView);
	}

	glm::vec2 EditorCamera::PanSpeed() const
	{
		f32 x = std::min(mViewportWidth / 1000.0f, 2.4f); // max = 2.4f
		f32 xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		f32 y = std::min(mViewportHeight / 1000.0f, 2.4f); // max = 2.4f
		f32 yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

		return { xFactor, yFactor };
	}

	f32 EditorCamera::RotationSpeed() const
	{
		return 0.8f;
	}

	f32 EditorCamera::ZoomSpeed() const
	{
		f32 distance = mDistance * 0.2f;
		distance = std::max(distance, 0.0f);
		f32 speed = distance * distance;
		speed = std::min(speed, 100.0f); // max speed = 100
		return speed;
	}

	void EditorCamera::onUpdate(Timestep ts)
	{
		if (Input::IsKeyPressed(Key::LeftShift))
		{
			const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
			glm::vec2 delta = (mouse - mInitialMousePosition) * 0.003f;
			mInitialMousePosition = mouse;

			if (Input::IsMouseButtonPressed(Mouse::ButtonLeft))
				MousePan(delta);
			else if (Input::IsMouseButtonPressed(Mouse::ButtonMiddle))
				MouseRotate(delta);
			else if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
				MouseZoom(delta.y);
		}

		UpdateView();
	}

	void EditorCamera::onEvent(Event& e)
	{
		e.dispatch<MouseScrolledEvent>(LAB_BIND_EVENT_FUNC(EditorCamera::OnMouseScroll));
	}

	bool EditorCamera::OnMouseScroll(MouseScrolledEvent& e)
	{
		f32 delta = e.yOffset * 0.1f;
		MouseZoom(delta);
		UpdateView();
		return false;
	}

	void EditorCamera::MousePan(const glm::vec2& delta)
	{
		glm::vec2 speed = PanSpeed();
		mFocalPoint += -getRightDirection() * delta.x * speed.x * mDistance;
		mFocalPoint += getUpDirection() * delta.y * speed.y * mDistance;
	}

	void EditorCamera::MouseRotate(const glm::vec2& delta)
	{
		f32 yawSign = getUpDirection().y < 0 ? -1.0f : 1.0f;
		mYaw += yawSign * delta.x * RotationSpeed();
		mPitch += delta.y * RotationSpeed();
	}

	void EditorCamera::MouseZoom(f32 delta)
	{
		mDistance -= delta * ZoomSpeed();
		if (mDistance < 1.0f)
		{
			mFocalPoint += getForwardDirection();
			mDistance = 1.0f;
		}
	}

	glm::vec3 EditorCamera::getUpDirection() const
	{
		return glm::rotate(getOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::vec3 EditorCamera::getRightDirection() const
	{
		return glm::rotate(getOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glm::vec3 EditorCamera::getForwardDirection() const
	{
		return glm::rotate(getOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	glm::vec3 EditorCamera::CalculatePosition() const
	{
		return mFocalPoint - getForwardDirection() * mDistance;
	}

	glm::quat EditorCamera::getOrientation() const
	{
		return glm::quat(glm::vec3(-mPitch, -mYaw, 0.0f));
	}

}