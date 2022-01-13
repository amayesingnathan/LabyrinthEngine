#pragma once

#include "Camera.h"
#include "EditorCamera.h"

#include "Texture.h"
#include "SubTexture.h"

namespace Labyrinth {

	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginState(const Camera& camera, const glm::mat4 transform);
		static void BeginState(const OrthographicCamera& camera);
		static void BeginState(const EditorCamera& camera);
		static void EndState();
		static void Flush();

		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& colour);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& colour);
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColour = glm::vec4(1.0f), const glm::vec2* textureCoords = nullptr);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColour = glm::vec4(1.0f), const glm::vec2* textureCoords = nullptr);
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<SubTexture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColour = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<SubTexture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColour = glm::vec4(1.0f));

		static void DrawQuad(const glm::mat4& transform, const glm::vec4& colour);
		static void DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColour = glm::vec4(1.0f), const glm::vec2* textureCoords = nullptr);

		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& colour);
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& colour);
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColour = glm::vec4(1.0f), const glm::vec2* textureCoords = nullptr);
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColour = glm::vec4(1.0f), const glm::vec2* textureCoords = nullptr);
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColour = glm::vec4(1.0f));
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColour = glm::vec4(1.0f));

		// Stats
		struct Statistics
		{
			uint32_t drawCalls = 0;
			uint32_t quadCount = 0;

			uint32_t getTotalVertexCount() const { return quadCount * 4; }
			uint32_t getTotalIndexCount() const { return quadCount * 6; }
		};
		static void ResetStats();
		static Statistics GetStats();

	private:
		static void StartBatch();
		static void NextBatch();
	};
}