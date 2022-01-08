#include "Lpch.h"
#include "CameraController.h"

#include "Input.h"
#include "KeyCodes.h"
#include "MouseButtonCodes.h"

namespace Labyrinth {

	OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
		: mAspectRatio(aspectRatio), mCamera(-mAspectRatio * mZoom, mAspectRatio * mZoom, -mZoom, mZoom), mRotation(rotation)
	{
		mLastMousePos = { Input::GetMouseX(), Input::GetMouseY() };
	}

	void OrthographicCameraController::onUpdate(Timestep ts)
	{
		//Calculate how much to move camera from mouse being dragged.
		//Ignore keyboard input if dragging
		if (mDragging)
		{
			float moveX = mLastMousePos.x - Input::GetMouseX();
			float moveY = Input::GetMouseY() - mLastMousePos.y;
			//Scale dragging movement speed down for more intuitive feel
			mCameraPosition += 0.06f * mCameraTranslationSpeed * mZoom * ts * (mZoom * glm::vec3(moveX, moveY, 0.0f));

			mCamera.setPosition(mCameraPosition);
			mCamera.setZoom(mZoom);

			mLastMousePos.x = Input::GetMouseX();
			mLastMousePos.y = Input::GetMouseY();
			return;
		}

		mCameraRotationMat = mCamera.getRotationMat();
		if (Input::IsKeyPressed(LAB_KEY_LSHIFT)) mCameraRotationMat = glm::mat3(1.0f); //Lock movement to world axis if holding shift

		//Move the camera depending on it's current rotation for more intuitive movement.
		//Similarly scale camera move speed by zoom factor.
		if (Input::IsKeyPressed(LAB_KEY_A))
			mCameraPosition += (mCameraTranslationSpeed * mZoom) * ts * (mCameraRotationMat * glm::vec3(-1.0f, 0.0f, 0.0f));
		else if (Input::IsKeyPressed(LAB_KEY_D))
			mCameraPosition += (mCameraTranslationSpeed * mZoom) * ts * (mCameraRotationMat * glm::vec3(1.0f, 0.0f, 0.0f));

		if (Input::IsKeyPressed(LAB_KEY_W))
			mCameraPosition += (mCameraTranslationSpeed * mZoom) * ts * (mCameraRotationMat * glm::vec3(0.0f, 1.0f, 0.0f));
		else if (Input::IsKeyPressed(LAB_KEY_S))
			mCameraPosition += (mCameraTranslationSpeed * mZoom) * ts * (mCameraRotationMat * glm::vec3(0.0f, -1.0f, 0.0f));

		if (mRotation)
		{
			if (Input::IsKeyPressed(LAB_KEY_Q))
				mCameraRotation += mCameraRotationSpeed * ts;
			if (Input::IsKeyPressed(LAB_KEY_E))
				mCameraRotation -= mCameraRotationSpeed * ts;
			mCamera.setRotation(mCameraRotation);
		}

		mCamera.setPosition(mCameraPosition);
		mCamera.setZoom(mZoom);

		mLastMousePos.x = Input::GetMouseX();
		mLastMousePos.y = Input::GetMouseY();
	}

	void OrthographicCameraController::onEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<MouseScrolledEvent>(LAB_BIND_EVENT_FUNC(OrthographicCameraController::onMouseScrolled));
		dispatcher.dispatch<WindowResizeEvent>(LAB_BIND_EVENT_FUNC(OrthographicCameraController::onWindowResized));
		dispatcher.dispatch<WindowResizeEvent>(LAB_BIND_EVENT_FUNC(OrthographicCameraController::onWindowResized));
		dispatcher.dispatch<MouseButtonPressedEvent>(LAB_BIND_EVENT_FUNC(OrthographicCameraController::onMouseButtonPressed));
		dispatcher.dispatch<MouseButtonReleasedEvent>(LAB_BIND_EVENT_FUNC(OrthographicCameraController::onMouseButtonReleased));
	}

	bool OrthographicCameraController::onMouseScrolled(MouseScrolledEvent& e)
	{
		mZoom -= e.getYOffset() * 0.04f;
		mZoom = std::max(mZoom, 0.25f); 
		mCamera.setProjection(-mAspectRatio * mZoom, mAspectRatio * mZoom, -mZoom, mZoom);
		return false;
	}

	bool OrthographicCameraController::onWindowResized(WindowResizeEvent& e)
	{
		mAspectRatio = (float)e.getWidth() / (float)e.getHeight();
		mCamera.setProjection(-mAspectRatio * mZoom, mAspectRatio * mZoom, -mZoom, mZoom);
		return false;
	}

	bool OrthographicCameraController::onMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.getMouseButton() == LAB_MOUSE_BUTTON_LEFT)
			mDragging = true;
		return false;
	}

	bool OrthographicCameraController::onMouseButtonReleased(MouseButtonReleasedEvent& e)
	{
		if (e.getMouseButton() == LAB_MOUSE_BUTTON_LEFT)
			mDragging = false;
		return false;
	}

}