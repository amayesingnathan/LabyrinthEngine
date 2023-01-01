#include "Lpch.h"
#include "Renderer2D.h"

#include "Renderer.h"
#include "Camera.h"

namespace Laby {

	void Renderer2D::Init()
	{
		{	// Quads
			sRenderData.quadVertexArray = Ref<VertexArray>::Create();

			sRenderData.quadVertexBuffer = Ref<VertexBuffer>::Create(sRenderData.MaxVertices * (u32)sizeof(QuadVertex));
			sRenderData.quadVertexBuffer->setLayout({
				{ ShaderDataType::Float3, "aPosition"	  },
				{ ShaderDataType::Float4, "aColour"		  },
				{ ShaderDataType::Float2, "aTexCoord"	  },
				{ ShaderDataType::Float,  "aTexIndex"	  },
				{ ShaderDataType::Float,  "aTilingFactor" },
				{ ShaderDataType::Int,    "aEntityID"	  }
				});

			sRenderData.quadVertexArray->addVertexBuffer(sRenderData.quadVertexBuffer);

			sRenderData.quadVertexBufferBase = new QuadVertex[sRenderData.MaxVertices];

			u32* quadIndices = new u32[sRenderData.MaxIndices];

			u32 offset = 0;
			for (u32 i = 0; i < sRenderData.MaxIndices; i += 6)
			{
				quadIndices[i + 0] = offset + 0;
				quadIndices[i + 1] = offset + 1;
				quadIndices[i + 2] = offset + 2;

				quadIndices[i + 3] = offset + 2;
				quadIndices[i + 4] = offset + 3;
				quadIndices[i + 5] = offset + 0;

				offset += 4;
			}

			Ref<IndexBuffer> quadIB = Ref<IndexBuffer>::Create(quadIndices, sRenderData.MaxIndices);
			sRenderData.quadVertexArray->setIndexBuffer(quadIB);
			delete[] quadIndices;

			sRenderData.quadShader = Ref<Shader>::Create("resources/shaders/Renderer2DQuad.glsl");
		}

		// White Texture
		sRenderData.whiteTexture = Ref<Texture2D>::Create(1, 1);
		u32 whiteTextureData = 0xffffffff;
		sRenderData.whiteTexture->setData(&whiteTextureData, sizeof(u32));

		sRenderData.textureSlots[0] = sRenderData.whiteTexture;

		sRenderData.cameraUniformBuffer = Ref<UniformBuffer>::Create(sizeof(Renderer2DData::CameraData), 0);
	}

	void Renderer2D::Shutdown()
	{
		delete[] sRenderData.quadVertexBufferBase;
	}

	void Renderer2D::BeginState()
	{
		sRenderData.cameraMatrix = glm::mat4(1.0f);
		sRenderData.cameraUniformBuffer->setData(&sRenderData.cameraMatrix, sizeof(Renderer2DData::CameraData));

		StartBatch();
	}

	void Renderer2D::BeginState(const Camera& camera)
	{
		sRenderData.cameraMatrix = camera.getViewProjection();
		sRenderData.cameraUniformBuffer->setData(&sRenderData.cameraMatrix, sizeof(Renderer2DData::CameraData));

		StartBatch();
	}

	void Renderer2D::EndState()
	{
		Flush();
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& colour, i32 entityID)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), { position, 0.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, colour, entityID);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<IRenderable>& texture, f32 tilingFactor, const glm::vec4& tintColour, i32 entityID)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), { position, 0.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tilingFactor, tintColour, entityID);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, f32 rotation, const glm::vec4& colour, i32 entityID)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), { position, 0.0f })
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, colour, entityID);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, f32 rotation, const Ref<IRenderable>& texture, f32 tilingFactor, const glm::vec4& tintColour, i32 entityID)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), { position, 0.0f })
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tilingFactor, tintColour, entityID);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& colour, i32 entityID)
	{
		constexpr usize quadVertexCount = 4;
		constexpr glm::vec2 textureCoords[4] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		constexpr f32 textureIndex = 0.0f; // White Texture
		constexpr f32 tilingFactor = 1.0f;

		if (sRenderData.quadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		for (usize i = 0; i < quadVertexCount; i++)
		{
			sRenderData.quadVertexBufferPtr->position = transform * Renderer2DData::QuadVertexPositions[i];
			sRenderData.quadVertexBufferPtr->colour = colour;
			sRenderData.quadVertexBufferPtr->texCoord = textureCoords[i];
			sRenderData.quadVertexBufferPtr->texIndex = textureIndex;
			sRenderData.quadVertexBufferPtr->tilingFactor = tilingFactor;
			sRenderData.quadVertexBufferPtr->entityID = entityID;
			sRenderData.quadVertexBufferPtr++;
		}

		sRenderData.quadIndexCount += 6;
		sRenderData.stats.quadCount++;
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<IRenderable>& textureSlot, f32 tilingFactor, const glm::vec4& tintColour, i32 entityID)
	{
		constexpr usize quadVertexCount = 4;
		constexpr glm::vec2 defaultTextureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		const glm::vec2* textureCoords = textureSlot->getTextureCoords();

		if (!textureCoords)
			textureCoords = defaultTextureCoords;

		if (sRenderData.quadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		f32 textureIndex = 0.0f;
		for (u32 i = 1; i < sRenderData.textureSlotIndex; i++)
		{
			if (*sRenderData.textureSlots[i] == *textureSlot)
			{
				textureIndex = (f32)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if (sRenderData.textureSlotIndex >= Renderer2DData::MaxTextureSlots)
				NextBatch();

			textureIndex = (f32)sRenderData.textureSlotIndex;
			sRenderData.textureSlots[sRenderData.textureSlotIndex] = textureSlot;
			sRenderData.textureSlotIndex++;
		}

		for (usize i = 0; i < quadVertexCount; i++)
		{
			sRenderData.quadVertexBufferPtr->position = transform * Renderer2DData::QuadVertexPositions[i];
			sRenderData.quadVertexBufferPtr->colour = tintColour;
			sRenderData.quadVertexBufferPtr->texCoord = textureCoords[i];
			sRenderData.quadVertexBufferPtr->texIndex = textureIndex;
			sRenderData.quadVertexBufferPtr->tilingFactor = tilingFactor;
			sRenderData.quadVertexBufferPtr->entityID = entityID;
			sRenderData.quadVertexBufferPtr++;
		}

		sRenderData.quadIndexCount += 6;
		sRenderData.stats.quadCount++;
	}

	void Renderer2D::StartBatch()
	{
		sRenderData.quadIndexCount = 0;
		sRenderData.quadVertexBufferPtr = sRenderData.quadVertexBufferBase;

		sRenderData.textureSlotIndex = 1;
	}

	void Renderer2D::Flush()
	{
		// Quads
		if (sRenderData.quadIndexCount)
		{
			u32 quadDataSize = (u32)((u8*)sRenderData.quadVertexBufferPtr - (u8*)sRenderData.quadVertexBufferBase);
			sRenderData.quadVertexBuffer->setData(sRenderData.quadVertexBufferBase, quadDataSize);

			for (u32 i = 0; i < sRenderData.textureSlotIndex; i++)
				sRenderData.textureSlots[i]->bind(i);

			sRenderData.quadShader->bind();
			Renderer::DrawIndexed(sRenderData.quadVertexArray, sRenderData.quadIndexCount);
			sRenderData.stats.drawCalls++;
		}
	}

	void Renderer2D::NextBatch()
	{
		Flush();
		StartBatch();
	}

	void Renderer2D::ResetStats()
	{
		memset(&sRenderData.stats, 0, sizeof(RenderStatistics));
	}

	const RenderStatistics& Renderer2D::GetStats()
	{
		return sRenderData.stats;
	}
}
