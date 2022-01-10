#pragma once

#include "Labyrinth/Renderer/Camera.h"
#include "Labyrinth/Core/Timestep.h"

#include "Labyrinth/Events/ApplicationEvent.h"
#include "Labyrinth/Events/MouseEvent.h"

namespace Labyrinth {

	class OrthographicCameraController
	{
	public:
		OrthographicCameraController(float aspectRatio, bool rotation = false, bool dragging = false);

		void onUpdate(Timestep ts);
		void onEvent(Event& e);

		OrthographicCamera& getCamera() { return mCamera; }
		const OrthographicCamera& getCamera() const { return mCamera; }

		float getZoom() const { return mZoom; }
		void setZoom(float zoom) { mZoom = zoom; }

	private:
		bool onMouseScrolled(MouseScrolledEvent& e);
		bool onWindowResized(WindowResizeEvent& e);
		bool onMouseButtonPressed(MouseButtonPressedEvent& e);
		bool onMouseButtonReleased(MouseButtonReleasedEvent& e);

	private:
		float mAspectRatio;
		float mZoom = 1.0f;
		OrthographicCamera mCamera;

		bool mRotation;
		bool mDraggable;

		bool mDragging = false;
		bool mLockCam = false;

		glm::vec3 mCameraPosition = { 0.0f, 0.0f, 0.0f };
		float mCameraRotation = 0.0f;
		float mCameraTranslationSpeed = 2.5f, mCameraRotationSpeed = 90.0f;

		glm::vec2 mLastMousePos;
	};
}