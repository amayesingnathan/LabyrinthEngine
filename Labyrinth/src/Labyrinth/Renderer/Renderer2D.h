#pragma once

#include "IRenderable.h"
#include "Renderer2DInternal.h"
#include "RenderLayer.h"

namespace Laby {

	class Camera;
	class OrthographicCamera;
	class EditorCamera;

	struct TransformComponent;
	struct SpriteRendererComponent;
	struct CircleRendererComponent;

	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginState();
		static void BeginState(const glm::mat4& transform);
		static void BeginState(const Camera& camera, const glm::mat4& transform);
		static void BeginState(const Camera& camera);
		static void EndState();

		static void DrawSprite(const TransformComponent& transform, const SpriteRendererComponent& src, i32 entityID);
		static void DrawCircle(const TransformComponent& transform, const CircleRendererComponent& crc, i32 entityID = -1);

		static void DrawElement(const DrawData& drawData);

		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& colour, i32 entityID = -1);
		static void DrawQuad(const glm::mat4& transform, const glm::vec4& colour, i32 entityID = -1);

		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<IRenderable>& texture, f32 tilingFactor = 1.0f, const glm::vec4& tintColour = glm::vec4(1.0f), i32 entityID = -1);
		static void DrawQuad(const glm::mat4& transform, const Ref<IRenderable>& textureSlot, f32 tilingFactor = 1.0f, const glm::vec4& tintColour = glm::vec4(1.0f), i32 entityID = -1);

		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, f32 rotation, const glm::vec4& colour, i32 entityID = -1);
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, f32 rotation, const Ref<IRenderable>& texture, f32 tilingFactor = 1.0f, const glm::vec4& tintColour = glm::vec4(1.0f), i32 entityID = -1);

		static void DrawCircle(const glm::mat4& transform, const glm::vec4& colour, f32 thickness = 1.0f, i32 entityID = -1);

		static void DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& colour, i32 entityID = -1);

		static void DrawRect(const glm::vec2& position, const glm::vec2& size, const glm::vec4& colour, i32 entityID = -1);
		static void DrawRect(const glm::mat4& transform, const glm::vec4& colour, i32 entityID = -1);

		static void ResetStats();
		static const RenderStatistics& GetStats();

	private:
		static void DrawQuad(const glm::mat4& trans, RenderType type, AssetHandle handle, const glm::vec4 colour, f32 tilingFactor, i32 entityID);

	private:
		static void StartBatch();
		static void Flush();
		static void NextBatch();

	private:
		inline static Single<Renderer2DData> sRenderData;
	};
}