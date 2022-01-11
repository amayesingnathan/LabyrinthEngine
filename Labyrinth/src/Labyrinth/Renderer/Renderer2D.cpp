#include "Lpch.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Labyrinth {

	struct quadVertex
	{
		glm::vec3 position;
		glm::vec4 colour;
		glm::vec2 texCoord;

		float texIndex;
		float tilingFactor;
	};

	struct Renderer2DData
	{
		static const uint32_t Maxquads = 20000;
		static const uint32_t MaxVertices = Maxquads * 4;
		static const uint32_t MaxIndices = Maxquads * 6;
		static const uint32_t MaxTextureSlots = 32; //ToDo: RenderCaps

		Ref<VertexArray> quadVertexArray;
		Ref<VertexBuffer> quadVertexBuffer;
		Ref<Shader> textureShader;
		Ref<Texture2D> whiteTexture;

		uint32_t quadIndexCount = 0;
		quadVertex* quadVertexBufferBase = nullptr;
		quadVertex* quadVertexBufferPtr = nullptr;

		std::array<Ref<Texture2D>, MaxTextureSlots> textureSlots;
		uint32_t textureSlotIndex;

		glm::vec4 quadVertexPositions[4]; 

		Renderer2D::Statistics stats;
	};

	static Renderer2DData sData;

	void Renderer2D::Init()
	{
		LAB_PROFILE_FUNCTION();

		sData.quadVertexArray = VertexArray::Create();

		sData.quadVertexBuffer = VertexBuffer::Create(sData.MaxVertices * sizeof(quadVertex));
		sData.quadVertexBuffer->setLayout({
			{ ShaderDataType::Float3, "aPosition" },
			{ ShaderDataType::Float4, "aColour" },
			{ ShaderDataType::Float2, "aTexCoord" },
			{ ShaderDataType::Float, "aTexIndex" },
			{ ShaderDataType::Float, "aTilingFactor" }
			});

		sData.quadVertexArray->addVertexBuffer(sData.quadVertexBuffer);

		sData.quadVertexBufferBase = new quadVertex[sData.MaxVertices];

		uint32_t* quadIndices = new uint32_t[sData.MaxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < sData.MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}
				
		Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, sData.MaxIndices);
		sData.quadVertexArray->setIndexBuffer(quadIB);

		sData.whiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		sData.whiteTexture->setData(&whiteTextureData, sizeof(uint32_t));

		int32_t samplers[sData.MaxTextureSlots];
		for (uint32_t i = 0; i < sData.MaxTextureSlots; i++)
			samplers[i] = i;

		sData.textureShader = Shader::Create("assets/shaders/texture.glsl");
		sData.textureShader->bind();
		sData.textureShader->setIntArray("uTextures", samplers, sData.MaxTextureSlots);

		sData.textureSlots[0] = sData.whiteTexture;

		sData.quadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		sData.quadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
		sData.quadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
		sData.quadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
	}

	void Renderer2D::Shutdown()
	{
		LAB_PROFILE_FUNCTION();

		delete[] sData.quadVertexBufferBase;
	}

	void Renderer2D::BeginState(const Camera& camera, const glm::mat4 transform)
	{
		LAB_PROFILE_FUNCTION();

		glm::mat4 viewProj = camera.getProjection() * glm::inverse(transform);

		sData.textureShader->bind();
		sData.textureShader->setMat4("uViewProjection", viewProj);

		StartBatch();
	}

	void Renderer2D::BeginState(const OrthographicCamera& camera)
	{
		LAB_PROFILE_FUNCTION();

		sData.textureShader->bind();
		sData.textureShader->setMat4("uViewProjection", camera.getViewProjectionMatrix());

		StartBatch();
	}

	void Renderer2D::EndState()
	{
		LAB_PROFILE_FUNCTION();

		uint32_t dataSize = (uint32_t)((uint8_t*)sData.quadVertexBufferPtr - (uint8_t*)sData.quadVertexBufferBase);
		sData.quadVertexBuffer->setData(sData.quadVertexBufferBase, dataSize);

		Flush();
	}
	
	void Renderer2D::StartBatch()
	{
		sData.quadIndexCount = 0;
		sData.quadVertexBufferPtr = sData.quadVertexBufferBase;

		sData.textureSlotIndex = 1;
	}

	void Renderer2D::Flush()
	{
		if (sData.quadIndexCount == 0)
			return; //Nothing to draw

		uint32_t dataSize = (uint32_t)((uint8_t*)sData.quadVertexBufferPtr - (uint8_t*)sData.quadVertexBufferBase);
		sData.quadVertexBuffer->setData(sData.quadVertexBufferBase, dataSize);

		for (uint32_t i = 0; i < sData.textureSlotIndex; i++)
			sData.textureSlots[i]->bind(i);

		RenderCommand::DrawIndexed(sData.quadVertexArray, sData.quadIndexCount);
		sData.stats.drawCalls++;
	}

	void Renderer2D::NextBatch()
	{
		Flush();
		StartBatch();
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& colour)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, colour);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& colour)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, colour);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColour)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, tilingFactor, tintColour);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColour)
	{
		LAB_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tilingFactor, tintColour);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& colour)
	{
		LAB_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;
		const float textureIndex = 0.0f; // White Texture
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		const float tilingFactor = 1.0f;

		if (sData.quadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			sData.quadVertexBufferPtr->position = transform * sData.quadVertexPositions[i];
			sData.quadVertexBufferPtr->colour = colour;
			sData.quadVertexBufferPtr->texCoord = textureCoords[i];
			sData.quadVertexBufferPtr->texIndex = textureIndex;
			sData.quadVertexBufferPtr->tilingFactor = tilingFactor;
			sData.quadVertexBufferPtr++;
		}

		sData.quadIndexCount += 6;

		sData.stats.quadCount++;
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColour)
	{
		LAB_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		if (sData.quadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < sData.textureSlotIndex; i++)
		{
			if (*sData.textureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if (sData.textureSlotIndex >= Renderer2DData::MaxTextureSlots)
				NextBatch();

			textureIndex = (float)sData.textureSlotIndex;
			sData.textureSlots[sData.textureSlotIndex] = texture;
			sData.textureSlotIndex++;
		}

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			sData.quadVertexBufferPtr->position = transform * sData.quadVertexPositions[i];
			sData.quadVertexBufferPtr->colour = tintColour;
			sData.quadVertexBufferPtr->texCoord = textureCoords[i];
			sData.quadVertexBufferPtr->texIndex = textureIndex;
			sData.quadVertexBufferPtr->tilingFactor = tilingFactor;
			sData.quadVertexBufferPtr++;
		}

		sData.quadIndexCount += 6;

		sData.stats.quadCount++;
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& colour)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, colour);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& colour)
	{
		LAB_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, colour);

	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColour)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, tilingFactor, tintColour);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColour)
	{
		LAB_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tilingFactor, tintColour);
	}

	void Renderer2D::ResetStats()
	{
		memset(&sData.stats, 0, sizeof(Statistics));
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return sData.stats;
	}


}
