#pragma once

#include "Camera.h"

#include "Texture.h"

namespace Labyrinth {

	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginState(const OrthographicCamera& camera);
		static void EndState();

		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& colour);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& colour);
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture);

	};
}