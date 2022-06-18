#pragma once

#include <glm/glm.hpp>

namespace Labyrinth {

	class OrthographicCamera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top);

		void setProjection(float left, float right, float bottom, float top);

		const glm::vec3& getPosition() const { return mPosition; }
		void setPosition(const glm::vec3& position) { mPosition = position; RecalculateViewMatrix(); }

		float getRotation() const { return mRotation; }
		void setRotation(float rotation) { mRotation = rotation; RecalculateViewMatrix(); }

		const glm::mat4& getProjectionMatrix() const { return mProjectionMatrix; }
		const glm::mat4& getViewMatrix() const { return mViewMatrix; }
		const glm::mat4& getViewProjectionMatrix() const { return mViewProjectionMatrix; }

	private:
		void RecalculateViewMatrix();

	private:
		glm::mat4 mProjectionMatrix;
		glm::mat4 mViewMatrix;
		glm::mat4 mViewProjectionMatrix;

		glm::vec3 mPosition = { 0.0f, 0.0f, 0.0f };
		float mRotation = 0.0f;
	};

}
