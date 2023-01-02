#pragma once

#include <Labyrinth/Core/System/Base.h>

#include "Camera.h"

namespace Laby {

	class OrthographicCamera : public Camera
	{
	public:
		OrthographicCamera(f32 left, f32 right, f32 bottom, f32 top);

		glm::mat4 getViewProjection() const override { return mProjection * mView; }

		const glm::vec3& getPosition() const { return mPosition; }
		void setPosition(const glm::vec3& position) { mPosition = position; RecalculateViewMatrix(); }

		f32 getRotation() const { return mRotation; }
		void setRotation(f32 rotation) { mRotation = rotation; RecalculateViewMatrix(); }

	private:
		void RecalculateViewMatrix();

	private:
		glm::mat4 mView;

		glm::vec3 mPosition = { 0.0f, 0.0f, 0.0f };
		f32 mRotation = 0.0f;
	};

}
