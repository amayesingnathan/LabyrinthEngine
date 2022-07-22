#pragma once

#include <glm/glm.hpp>

namespace Labyrinth {

	class OrthographicCamera
	{
	public:
		OrthographicCamera(f32 left, f32 right, f32 bottom, f32 top);

		void setProjection(f32 left, f32 right, f32 bottom, f32 top);

		const glm::vec3& getPosition() const { return mPosition; }
		void setPosition(const glm::vec3& position) { mPosition = position; RecalculateViewMatrix(); }

		f32 getRotation() const { return mRotation; }
		void setRotation(f32 rotation) { mRotation = rotation; RecalculateViewMatrix(); }

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
		f32 mRotation = 0.0f;
	};

}
