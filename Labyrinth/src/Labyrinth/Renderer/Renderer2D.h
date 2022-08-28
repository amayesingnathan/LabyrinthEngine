#pragma once

#include "Camera.h"
#include "EditorCamera.h"
#include "OrthographicCamera.h"

#include "Texture.h"
#include "SubTexture.h"
#include "Framebuffer.h"
#include "TextureSlot.h"

#include "Labyrinth/Scene/Components.h"

namespace Labyrinth {

	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginState();
		static void BeginState(const Camera& camera, const glm::mat4 transform);
		static void BeginState(const OrthographicCamera& camera);
		static void BeginState(const EditorCamera& camera);
		static void EndState();
		static void Flush();

		static void DrawFramebuffer(Ref<Framebuffer> framebuffer);

		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& colour);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& colour);
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, f32 tilingFactor = 1.0f, const glm::vec4& tintColour = glm::vec4(1.0f), const glm::vec2* textureCoords = nullptr);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, f32 tilingFactor = 1.0f, const glm::vec4& tintColour = glm::vec4(1.0f), const glm::vec2* textureCoords = nullptr);
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<SubTexture2D>& texture, f32 tilingFactor = 1.0f, const glm::vec4& tintColour = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<SubTexture2D>& texture, f32 tilingFactor = 1.0f, const glm::vec4& tintColour = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Framebuffer>& fbo, f32 tilingFactor = 1.0f, const glm::vec4& tintColour = glm::vec4(1.0f), const glm::vec2* textureCoords = nullptr);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Framebuffer>& fbo, f32 tilingFactor = 1.0f, const glm::vec4& tintColour = glm::vec4(1.0f), const glm::vec2* textureCoords = nullptr);

		static void DrawQuad(const glm::mat4& transform, const glm::vec4& colour, i32 entityID = -1);
		static void DrawQuad(const glm::mat4& transform, const Ref<TextureSlot>& textureSlot, f32 tilingFactor = 1.0f, const glm::vec4& tintColour = glm::vec4(1.0f), const glm::vec2* textureCoords = nullptr, i32 entityID = -1);

		static void DrawSprite(const glm::mat4& transform, const SpriteRendererComponent& src, i32 entityID);
		static void DrawMap(const glm::mat4& transform, const TilemapControllerComponent& tmcc, i32 entityID);

		// Rotation in degrees
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, f32 rotation, const glm::vec4& colour);
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, f32 rotation, const glm::vec4& colour);
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, f32 rotation, const Ref<Texture2D>& texture, f32 tilingFactor = 1.0f, const glm::vec4& tintColour = glm::vec4(1.0f), const glm::vec2* textureCoords = nullptr);
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, f32 rotation, const Ref<Texture2D>& texture, f32 tilingFactor = 1.0f, const glm::vec4& tintColour = glm::vec4(1.0f), const glm::vec2* textureCoords = nullptr);
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, f32 rotation, const Ref<SubTexture2D>& texture, f32 tilingFactor = 1.0f, const glm::vec4& tintColour = glm::vec4(1.0f));
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, f32 rotation, const Ref<SubTexture2D>& texture, f32 tilingFactor = 1.0f, const glm::vec4& tintColour = glm::vec4(1.0f));
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, f32 rotation, const Ref<Framebuffer>& fbo, f32 tilingFactor = 1.0f, const glm::vec4& tintColour = glm::vec4(1.0f), const glm::vec2* textureCoords = nullptr);
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, f32 rotation, const Ref<Framebuffer>& fbo, f32 tilingFactor = 1.0f, const glm::vec4& tintColour = glm::vec4(1.0f), const glm::vec2* textureCoords = nullptr);

		static void DrawCircle(const glm::mat4& transform, const glm::vec4& colour, f32 thickness = 1.0f, i32 entityID = -1);
		static void DrawCircle(const glm::mat4& transform, const CircleRendererComponent& src, i32 entityID = -1);

		static void DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& colour, i32 entityID = -1);

		static void DrawRect(const glm::vec2& position, const glm::vec2& size, const glm::vec4& colour, i32 entityID = -1);
		static void DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& colour, i32 entityID = -1);
		static void DrawRect(const glm::mat4& transform, const glm::vec4& colour, i32 entityID = -1);

		static f32 GetLineWidth();
		static void SetLineWidth(f32 width);

		// Stats
		struct Statistics
		{
			u32 drawCalls = 0;
			u32 quadCount = 0;

			u32 getTotalVertexCount() const { return quadCount * 4; }
			u32 getTotalIndexCount() const { return quadCount * 6; }
		};
		static void ResetStats();
		static Statistics GetStats();

	private:
		static void StartBatch();
		static void NextBatch();
	};
}