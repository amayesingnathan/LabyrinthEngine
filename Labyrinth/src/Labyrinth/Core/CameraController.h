#pragma once

#include "Labyrinth/Renderer/OrthographicCamera.h"
#include "Labyrinth/Core/Timestep.h"

#include "Labyrinth/Events/ApplicationEvent.h"
#include "Labyrinth/Events/MouseEvent.h"

namespace Labyrinth {

	class LAB_API OrthographicCameraController
	{
	public:
		OrthographicCameraController(f32 aspectRatio, bool rotation = false, bool dragging = false);

		void onUpdate(Timestep ts);
		void onEvent(Event& e);

		void onResize(f32 width, f32 height);

		OrthographicCamera& getCamera() { return mCamera; }
		const OrthographicCamera& getCamera() const { return mCamera; }

		f32 getZoom() const { return mZoom; }
		void setZoom(f32 zoom) { mZoom = zoom; }

	private:
		bool onMouseScrolled(MouseScrolledEvent& e);
		bool onWindowResized(WindowResizeEvent& e);
		bool onMouseButtonPressed(MouseButtonPressedEvent& e);
		bool onMouseButtonReleased(MouseButtonReleasedEvent& e);

	private:
		f32 mAspectRatio;
		f32 mZoom = 1.0f;
		OrthographicCamera mCamera;

		bool mRotation;
		bool mDraggable;

		bool mDragging = false;
		bool mLockCam = false;

		glm::vec3 mCameraPosition = { 0.0f, 0.0f, 0.0f };
		f32 mCameraRotation = 0.0f;
		f32 mCameraTranslationSpeed = 2.5f, mCameraRotationSpeed = 90.0f;

		glm::vec2 mLastMousePos;
	};
}