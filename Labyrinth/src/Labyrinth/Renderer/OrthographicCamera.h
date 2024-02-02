#pragma once

#include <Labyrinth/Core/System/Base.h>

#include "Camera.h"

namespace Laby {

	class OrthographicCamera : public Camera
	{
	public:
		OrthographicCamera(f32 left, f32 right, f32 bottom, f32 top);

		glm::mat4 GetViewProjection() const override { return mProjection * mView; }

		const glm::vec3& GetPosition() const { return mPosition; }
		void SetPosition(const glm::vec3& position) { mPosition = position; RecalculateViewMatrix(); }

		f32 GetRotation() const { return mRotation; }
		void SetRotation(f32 rotation) { mRotation = rotation; RecalculateViewMatrix(); }

	private:
		void RecalculateViewMatrix();

	private:
		glm::mat4 mView;

		glm::vec3 mPosition = { 0.0f, 0.0f, 0.0f };
		f32 mRotation = 0.0f;
	};

}
