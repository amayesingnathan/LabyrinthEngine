#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Labyrinth/Core/System/Base.h>

#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"
#include "UniformBuffer.h"

namespace Laby {

	struct QuadVertex
	{
		glm::vec3 position;
		glm::vec4 colour;
		glm::vec2 texCoord;

		f32 texIndex;
		f32 tilingFactor;

		// Editor-only
		i32 entityID;
	};

	struct RenderStatistics
	{
		u32 drawCalls = 0;
		u32 quadCount = 0;

		u32 getTotalVertexCount() const { return quadCount * 4; }
		u32 getTotalIndexCount() const { return quadCount * 6; }
	};

	struct Renderer2DData
	{
		static constexpr u32 MaxQuads = 20000;
		static constexpr u32 MaxVertices = MaxQuads * 4;
		static constexpr u32 MaxIndices = MaxQuads * 6;
		static constexpr u32 MaxTextureSlots = 32;
		static constexpr glm::vec4 QuadVertexPositions[4] = 
		{ 
			{ -0.5f, -0.5f, 0.0f, 1.0f },
			{  0.5f, -0.5f, 0.0f, 1.0f },
			{  0.5f,  0.5f, 0.0f, 1.0f },
			{ -0.5f,  0.5f, 0.0f, 1.0f } 
		};

		Ref<VertexArray> quadVertexArray;
		Ref<VertexBuffer> quadVertexBuffer;
		Ref<Shader> quadShader;

		u32 quadIndexCount = 0;
		QuadVertex* quadVertexBufferBase = nullptr;
		QuadVertex* quadVertexBufferPtr = nullptr;

		Ref<Texture2D> whiteTexture;
		std::array<Ref<IRenderable>, MaxTextureSlots> textureSlots;
		u32 textureSlotIndex;

		RenderStatistics stats;

		using CameraData = glm::mat4;
		CameraData cameraMatrix;
		Ref<UniformBuffer> cameraUniformBuffer;
	};
}