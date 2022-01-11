#pragma once

#include <glm/glm.hpp>

namespace Labyrinth {

	class Camera
	{
	public:
		Camera() = default;
		Camera(const glm::mat4& projection)
			: mProjectionMatrix(projection) {}

		virtual ~Camera() = default;

		const glm::mat4& getProjection() const { return mProjectionMatrix; }

	protected:
		glm::mat4 mProjectionMatrix = glm::mat4(1.0f);
	};

	class OrthographicCamera : public Camera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top, float zoom = 1.0f);
		virtual ~OrthographicCamera() override = default;

		void setProjection(float left, float right, float bottom, float top);

		const glm::vec3& getPosition() const { return mPosition; }
		void setPosition(const glm::vec3& position) {  mPosition = position; recalculateViewMatrix(); }

		float getRotation() const { return mRotation; }
		void setRotation(float rotation) {  mRotation = rotation; recalculateViewMatrix(); }

		float getZoom() const { return mZoom; }
		void setZoom(float zoom) { mZoom = zoom; recalculateViewMatrix(); }

		const glm::mat4& getViewMatrix() const { return mViewMatrix; }
		const glm::mat4& getViewProjectionMatrix() const { return mViewProjectionMatrix; }

	private:
		void recalculateViewMatrix();

	private:
		glm::mat4 mViewMatrix;
		glm::mat4 mViewProjectionMatrix;

		glm::vec3 mPosition = { 0.0f, 0.0f, 0.0f };
		float mRotation = 0.0f;
		float mZoom = 1.0f;
	};


}