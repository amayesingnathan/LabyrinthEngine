#pragma once

#include <glm/glm.hpp>

namespace Labyrinth {

	class OrthographicCamera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top, float zoom = 1.0f);

		const glm::vec3& getPosition() const { return mPosition; }
		void setPosition(const glm::vec3& position) {  mPosition = position; recalculateViewMatrix(); }

		float getRotation() const { return mRotation; }
		void setRotation(float rotation) {  mRotation = rotation; recalculateViewMatrix(); }

		float getZoom() const { return mZoom; }
		void setZoom(float zoom) { mZoom = zoom; recalculateViewMatrix(); }

		const glm::mat3 getRotationMat() const { return mRotationMat; }

		const glm::mat4& getProjectionMatrix() const { return mProjectionMatrix; }
		const glm::mat4& getViewMatrix() const { return mViewMatrix; }
		const glm::mat4& getViewProjectionMatrix() const { return mViewProjectionMatrix; }

	private:
		void recalculateViewMatrix();

	private:
		glm::mat4 mProjectionMatrix;
		glm::mat4 mViewMatrix;
		glm::mat4 mViewProjectionMatrix;

		glm::vec4 mStartScale;
		glm::vec3 mPosition = { 0.0f, 0.0f, 0.0f };
		float mRotation = 0.0f;
		float mZoom = 1.0f;
		glm::mat3 mRotationMat;
	};

}