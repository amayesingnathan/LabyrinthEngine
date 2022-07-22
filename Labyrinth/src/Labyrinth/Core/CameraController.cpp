#include "Lpch.h"
#include "CameraController.h"

#include "Labyrinth/IO/Input.h"

namespace Labyrinth {

	OrthographicCameraController::OrthographicCameraController(f32 aspectRatio, bool rotation, bool dragging)
		: mAspectRatio(aspectRatio), mCamera(-mAspectRatio * mZoom, mAspectRatio * mZoom, -mZoom, mZoom), mRotation(rotation), mDraggable(dragging)
	{
		mLastMousePos = { Input::GetMouseX(), Input::GetMouseY() };
	}

	void OrthographicCameraController::onUpdate(Timestep ts)
	{
		LAB_PROFILE_FUNCTION();

		//Move the camera depending on it's current rotation for more intuitive movement.
		//Similarly scale camera move speed by zoom factor.
		if (Input::IsKeyPressed(Key::A))
		{
			mCameraPosition.x -= cos(glm::radians(mCameraRotation)) * mCameraTranslationSpeed * ts;
			mCameraPosition.y -= sin(glm::radians(mCameraRotation)) * mCameraTranslationSpeed * ts;
		}
		else if (Input::IsKeyPressed(Key::D))
		{
			mCameraPosition.x += cos(glm::radians(mCameraRotation)) * mCameraTranslationSpeed * ts;
			mCameraPosition.y += sin(glm::radians(mCameraRotation)) * mCameraTranslationSpeed * ts;
		}

		if (Input::IsKeyPressed(Key::W))
		{
			mCameraPosition.x += -sin(glm::radians(mCameraRotation)) * mCameraTranslationSpeed * ts;
			mCameraPosition.y += cos(glm::radians(mCameraRotation)) * mCameraTranslationSpeed * ts;
		}
		else if (Input::IsKeyPressed(Key::S))
		{
			mCameraPosition.x -= -sin(glm::radians(mCameraRotation)) * mCameraTranslationSpeed * ts;
			mCameraPosition.y -= cos(glm::radians(mCameraRotation)) * mCameraTranslationSpeed * ts;
		}

		if (mRotation)
		{
			if (Input::IsKeyPressed(Key::Q))
				mCameraRotation += mCameraRotationSpeed * ts;
			if (Input::IsKeyPressed(Key::E))
				mCameraRotation -= mCameraRotationSpeed * ts;

			if (mCameraRotation > 180.0f)
				mCameraRotation -= 360.0f;
			else if (mCameraRotation <= -180.0f)
				mCameraRotation += 360.0f;

			mCamera.setRotation(mCameraRotation);
		}

		mCamera.setPosition(mCameraPosition);

		mCameraTranslationSpeed = mZoom;
	}

	void OrthographicCameraController::onEvent(Event& e)
	{
		LAB_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.dispatch<MouseScrolledEvent>(LAB_BIND_EVENT_FUNC(OrthographicCameraController::onMouseScrolled));
		dispatcher.dispatch<WindowResizeEvent>(LAB_BIND_EVENT_FUNC(OrthographicCameraController::onWindowResized));
		dispatcher.dispatch<WindowResizeEvent>(LAB_BIND_EVENT_FUNC(OrthographicCameraController::onWindowResized));
		dispatcher.dispatch<MouseButtonPressedEvent>(LAB_BIND_EVENT_FUNC(OrthographicCameraController::onMouseButtonPressed));
		dispatcher.dispatch<MouseButtonReleasedEvent>(LAB_BIND_EVENT_FUNC(OrthographicCameraController::onMouseButtonReleased));
	}

	void OrthographicCameraController::onResize(f32 width, f32 height)
	{
		mAspectRatio = width / height;
		mCamera.setProjection(-mAspectRatio * mZoom, mAspectRatio * mZoom, -mZoom, mZoom);
	}

	bool OrthographicCameraController::onMouseScrolled(MouseScrolledEvent& e)
	{
		LAB_PROFILE_FUNCTION();

		mZoom -= e.getYOffset() * 0.04f;
		mZoom = std::min(mZoom, 4.0f); 
		mCamera.setProjection(-mAspectRatio * mZoom, mAspectRatio * mZoom, -mZoom, mZoom);
		return false;
	}

	bool OrthographicCameraController::onWindowResized(WindowResizeEvent& e)
	{
		LAB_PROFILE_FUNCTION();

		onResize((f32)e.getWidth(), (f32)e.getHeight());
		return false;
	}

	bool OrthographicCameraController::onMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.getMouseButton() == Mouse::ButtonLeft)
			if (mDraggable)
				mDragging = true;
		return false;
	}

	bool OrthographicCameraController::onMouseButtonReleased(MouseButtonReleasedEvent& e)
	{
		if (e.getMouseButton() == Mouse::ButtonLeft)
			mDragging = false;
		return false;
	}

}