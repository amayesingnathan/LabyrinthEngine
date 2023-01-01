#pragma once

#include <glm/glm.hpp>

#include "IRenderable.h"
#include "Renderer2DInternal.h"

namespace Laby {

	class Camera;
	class OrthographicCamera;
	class EditorCamera;

	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginState();
		static void BeginState(const Camera& camera);
		static void EndState();

		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& colour, i32 entityID = -1);
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<IRenderable>& texture, f32 tilingFactor = 1.0f, const glm::vec4& tintColour = glm::vec4(1.0f), i32 entityID = -1);

		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, f32 rotation, const glm::vec4& colour, i32 entityID = -1);
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, f32 rotation, const Ref<IRenderable>& texture, f32 tilingFactor = 1.0f, const glm::vec4& tintColour = glm::vec4(1.0f), i32 entityID = -1);

		static void ResetStats();
		static const RenderStatistics& GetStats();

	private:
		static void DrawQuad(const glm::mat4& transform, const glm::vec4& colour, i32 entityID = -1);
		static void DrawQuad(const glm::mat4& transform, const Ref<IRenderable>& textureSlot, f32 tilingFactor = 1.0f, const glm::vec4& tintColour = glm::vec4(1.0f), i32 entityID = -1);

		static void StartBatch();
		static void Flush();
		static void NextBatch();

	private:
		inline static Renderer2DData sRenderData;
	};
}