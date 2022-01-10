#include "Lpch.h"
#include "CameraController.h"

#include "Input.h"
#include "KeyCodes.h"
#include "MouseButtonCodes.h"

namespace Labyrinth {

	OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation, bool dragging)
		: mAspectRatio(aspectRatio), mCamera(-mAspectRatio * mZoom, mAspectRatio * mZoom, -mZoom, mZoom), mRotation(rotation), mDraggable(dragging)
	{
		mLastMousePos = { Input::GetMouseX(), Input::GetMouseY() };
	}

	void OrthographicCameraController::onUpdate(Timestep ts)
	{
		LAB_PROFILE_FUNCTION();

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

		if (mRotation)
		{
			if (Input::IsKeyPressed(LAB_KEY_Q))
				mCameraRotation += mCameraRotationSpeed * ts;
			if (Input::IsKeyPressed(LAB_KEY_E))
				mCameraRotation -= mCameraRotationSpeed * ts;

			if (mCameraRotation > 180.0f)
				mCameraRotation -= 360.0f;
			else if (mCameraRotation <= -180.0f)
				mCameraRotation += 360.0f;

			mCamera.setRotation(mCameraRotation);
		}

		//Lock movement to world axis if holding shift
		mLockCam = (Input::IsKeyPressed(LAB_KEY_LSHIFT)) ? true : false;

		//Move the camera depending on it's current rotation for more intuitive movement.
		//Similarly scale camera move speed by zoom factor.
		if (Input::IsKeyPressed(LAB_KEY_A))
		{
			if (!mLockCam)
			{
				mCameraPosition.x -= mCameraTranslationSpeed * mZoom * ts * cos(glm::radians(mCameraRotation));
				mCameraPosition.y -= mCameraTranslationSpeed * mZoom * ts * sin(glm::radians(mCameraRotation));
			}
			else mCameraPosition.x -= mCameraTranslationSpeed * mZoom * ts;
		}
		else if (Input::IsKeyPressed(LAB_KEY_D))
		{
			if (!mLockCam)
			{
				mCameraPosition.x += mCameraTranslationSpeed * mZoom * ts * cos(glm::radians(mCameraRotation));
				mCameraPosition.y += mCameraTranslationSpeed * mZoom * ts * sin(glm::radians(mCameraRotation));
			}
			else mCameraPosition.x += mCameraTranslationSpeed * mZoom * ts;
		}

		if (Input::IsKeyPressed(LAB_KEY_W))
		{
			if (!mLockCam)
			{
				mCameraPosition.x += mCameraTranslationSpeed * mZoom * ts * -sin(glm::radians(mCameraRotation));
				mCameraPosition.y += mCameraTranslationSpeed * mZoom * ts * cos(glm::radians(mCameraRotation));
			}
			else mCameraPosition.y += mCameraTranslationSpeed * mZoom * ts;
		}
		else if (Input::IsKeyPressed(LAB_KEY_S))
		{
			if (!mLockCam)
			{
				mCameraPosition.x -= mCameraTranslationSpeed * mZoom * ts * -sin(glm::radians(mCameraRotation));
				mCameraPosition.y -= mCameraTranslationSpeed * mZoom * ts * cos(glm::radians(mCameraRotation));
			}
			else mCameraPosition.y -= mCameraTranslationSpeed * mZoom * ts;
		}

		mCamera.setPosition(mCameraPosition);
		mCamera.setZoom(mZoom);

		mLastMousePos.x = Input::GetMouseX();
		mLastMousePos.y = Input::GetMouseY();
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

		mAspectRatio = (float)e.getWidth() / (float)e.getHeight();
		mCamera.setProjection(-mAspectRatio * mZoom, mAspectRatio * mZoom, -mZoom, mZoom);
		return false;
	}

	bool OrthographicCameraController::onMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.getMouseButton() == LAB_MOUSE_BUTTON_LEFT)
			if (mDraggable)
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