#include "Lpch.h"
#include "Renderer2D.h"

#include "Renderer.h"
#include "SubTexture.h"
#include "Camera.h"

#include <Labyrinth/Assets/AssetManager.h>
#include <Labyrinth/Scene/Components.h>
#include <Labyrinth/Tilemap/Tilemap.h>

namespace Laby {

	void Renderer2D::Init()
	{
		sRenderData = MakeSingle<Renderer2DData>();

		{	// Quads
			sRenderData->quadVertexArray = Ref<VertexArray>::Create();

			sRenderData->quadVertexBuffer = Ref<VertexBuffer>::Create(Renderer2DData::MaxVertices * (u32)sizeof(QuadVertex));
			sRenderData->quadVertexBuffer->SetLayout({
				{ ShaderDataType::Float3, "aPosition"	  },
				{ ShaderDataType::Float4, "aColour"		  },
				{ ShaderDataType::Float2, "aTexCoord"	  },
				{ ShaderDataType::Float,  "aTexIndex"	  },
				{ ShaderDataType::Float,  "aTilingFactor" },
				{ ShaderDataType::Int,    "aEntityID"	  }
				});

			sRenderData->quadVertexArray->AddVertexBuffer(sRenderData->quadVertexBuffer);

			sRenderData->quadVertexBufferBase = new QuadVertex[Renderer2DData::MaxVertices];

			u32* quadIndices = new u32[Renderer2DData::MaxIndices];

			u32 offset = 0;
			for (u32 i = 0; i < Renderer2DData::MaxIndices; i += 6)
			{
				quadIndices[i + 0] = offset + 0;
				quadIndices[i + 1] = offset + 1;
				quadIndices[i + 2] = offset + 2;

				quadIndices[i + 3] = offset + 2;
				quadIndices[i + 4] = offset + 3;
				quadIndices[i + 5] = offset + 0;

				offset += 4;
			}

			Ref<IndexBuffer> quadIB = Ref<IndexBuffer>::Create(quadIndices, Renderer2DData::MaxIndices);
			sRenderData->quadVertexArray->SetIndexBuffer(quadIB);
			delete[] quadIndices;

			sRenderData->quadShader = Ref<Shader>::Create("resources/shaders/Renderer2DQuad.glsl");
		}

		// Circles
		{
			sRenderData->circleVertexArray = Ref<VertexArray>::Create();

			sRenderData->circleVertexBuffer = Ref<VertexBuffer>::Create(sRenderData->MaxVertices * (u32)sizeof(QuadVertex));
			sRenderData->circleVertexBuffer->SetLayout({
				{ ShaderDataType::Float3, "aWorldPosition" },
				{ ShaderDataType::Float,  "aThickness"     },
				{ ShaderDataType::Float2, "aLocalPosition" },
				{ ShaderDataType::Float4, "aColour"        },
				{ ShaderDataType::Int,    "aEntityID"      }
				});

			sRenderData->circleVertexArray->AddVertexBuffer(sRenderData->circleVertexBuffer);

			sRenderData->circleVertexBufferBase = new CircleVertex[sRenderData->MaxVertices];
			sRenderData->circleVertexArray->SetIndexBuffer(sRenderData->quadVertexArray->GetIndexBuffer()); // Reuse quad index buffer

			sRenderData->circleShader = Ref<Shader>::Create("resources/shaders/Renderer2DCircle.glsl");
		}

		// Lines
		{
			sRenderData->lineVertexArray = Ref<VertexArray>::Create();

			sRenderData->lineVertexBuffer = Ref<VertexBuffer>::Create(sRenderData->MaxVertices * (u32)sizeof(QuadVertex));
			sRenderData->lineVertexBuffer->SetLayout({
				{ ShaderDataType::Float3, "aPosition" },
				{ ShaderDataType::Float4, "aColour"   },
				{ ShaderDataType::Int,    "aEntityID" }
				});

			sRenderData->lineVertexArray->AddVertexBuffer(sRenderData->lineVertexBuffer);
			sRenderData->lineVertexBufferBase = new LineVertex[sRenderData->MaxVertices];

			sRenderData->lineShader = Ref<Shader>::Create("resources/shaders/Renderer2DLine.glsl");
		}

		// White Texture
		sRenderData->whiteTexture = Ref<Texture2D>::Create(1, 1);
		u32 whiteTextureData = 0xffffffff;
		sRenderData->whiteTexture->SetData(&whiteTextureData, sizeof(u32));

		sRenderData->textureSlots[0] = sRenderData->whiteTexture;

		sRenderData->cameraUniformBuffer = Ref<UniformBuffer>::Create((u32)sizeof(Renderer2DData::CameraData), 0);
	}

	void Renderer2D::Shutdown()
	{
		delete[] sRenderData->quadVertexBufferBase;
		delete[] sRenderData->circleVertexBufferBase;
		delete[] sRenderData->lineVertexBufferBase;

		sRenderData.reset();
	}

	void Renderer2D::BeginState()
	{
		BeginState(glm::mat4{ 1.0f });
	}

	void Renderer2D::BeginState(const Camera& camera, const glm::mat4& transform)
	{
		BeginState(camera.GetProjection() * glm::inverse(transform));
	}

	void Renderer2D::BeginState(const Camera& camera)
	{
		BeginState(camera.GetViewProjection());
	}

	void Renderer2D::BeginState(const glm::mat4& cameraTransform)
	{
		sRenderData->cameraMatrix = cameraTransform;
		sRenderData->cameraUniformBuffer->SetData(&sRenderData->cameraMatrix, sizeof(Renderer2DData::CameraData));

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
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, colour, entityID);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, f32 rotation, const Ref<IRenderable>& texture, f32 tilingFactor, const glm::vec4& tintColour, i32 entityID)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), { position, 0.0f })
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tilingFactor, tintColour, entityID);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& colour, i32 entityID)
	{
		constexpr usize quadVertexCount = 4;
		constexpr glm::vec2 textureCoords[4] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		constexpr f32 textureIndex = 0.0f; // White Texture
		constexpr f32 tilingFactor = 1.0f;

		if (sRenderData->quadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		for (usize i = 0; i < quadVertexCount; i++)
		{
			sRenderData->quadVertexBufferPtr->position = transform * Renderer2DData::QuadVertexPositions[i];
			sRenderData->quadVertexBufferPtr->colour = colour;
			sRenderData->quadVertexBufferPtr->texCoord = textureCoords[i];
			sRenderData->quadVertexBufferPtr->texIndex = textureIndex;
			sRenderData->quadVertexBufferPtr->tilingFactor = tilingFactor;
			sRenderData->quadVertexBufferPtr->entityID = entityID;
			sRenderData->quadVertexBufferPtr++;
		}

		sRenderData->quadIndexCount += 6;
		sRenderData->stats.quadCount++;
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<IRenderable>& textureSlot, f32 tilingFactor, const glm::vec4& tintColour, i32 entityID)
	{
		constexpr usize quadVertexCount = 4;
		const glm::vec2* textureCoords = textureSlot->GetTextureCoords();

		if (sRenderData->quadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		f32 textureIndex = 0.0f;
		for (u32 i = 1; i < sRenderData->textureSlotIndex; i++)
		{
			if (*sRenderData->textureSlots[i] == *textureSlot)
			{
				textureIndex = (f32)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if (sRenderData->textureSlotIndex >= Renderer2DData::MaxTextureSlots)
				NextBatch();

			textureIndex = (f32)sRenderData->textureSlotIndex;
			sRenderData->textureSlots[sRenderData->textureSlotIndex] = textureSlot;
			sRenderData->textureSlotIndex++;
		}

		for (usize i = 0; i < quadVertexCount; i++)
		{
			sRenderData->quadVertexBufferPtr->position = transform * Renderer2DData::QuadVertexPositions[i];
			sRenderData->quadVertexBufferPtr->colour = tintColour;
			sRenderData->quadVertexBufferPtr->texCoord = textureCoords[i];
			sRenderData->quadVertexBufferPtr->texIndex = textureIndex;
			sRenderData->quadVertexBufferPtr->tilingFactor = tilingFactor;
			sRenderData->quadVertexBufferPtr->entityID = entityID;
			sRenderData->quadVertexBufferPtr++;
		}

		sRenderData->quadIndexCount += 6;
		sRenderData->stats.quadCount++;
	}

	void Renderer2D::DrawSprite(const TransformComponent& transform, const SpriteRendererComponent& src, i32 entityID)
	{
		DrawQuad(transform.GetTransform(), src.type, src.handle, src.colour, src.tilingFactor, entityID);
	}

	void Renderer2D::DrawCircle(const TransformComponent& transform, const CircleRendererComponent& crc, i32 entityID)
	{
		DrawCircle(transform, crc.colour, crc.thickness, entityID);
	}

	void Renderer2D::DrawElement(const DrawData& drawData)
	{
		switch (drawData.drawType)
		{
		case DrawData::Quad:
			DrawQuad(drawData.transform, drawData.texType, drawData.handle, drawData.colour, drawData.tilingFactorOrThickness, drawData.entityID);
			break;
		case DrawData::Circle:
			DrawCircle(drawData.transform, drawData.colour, drawData.tilingFactorOrThickness, drawData.entityID);
			break;
		}
	}

	void Renderer2D::DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& colour, i32 entityID)
	{
		sRenderData->lineVertexBufferPtr->position = p0;
		sRenderData->lineVertexBufferPtr->colour = colour;
		sRenderData->lineVertexBufferPtr->entityID = entityID;
		sRenderData->lineVertexBufferPtr++;

		sRenderData->lineVertexBufferPtr->position = p1;
		sRenderData->lineVertexBufferPtr->colour = colour;
		sRenderData->lineVertexBufferPtr->entityID = entityID;
		sRenderData->lineVertexBufferPtr++;

		sRenderData->lineVertexCount += 2;
	}

	void Renderer2D::DrawRect(const glm::vec2& position, const glm::vec2& size, const glm::vec4& colour, i32 entityID)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), { position, 0.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawRect(transform, colour, entityID);
	}

	void Renderer2D::DrawRect(const glm::mat4& transform, const glm::vec4& colour, i32 entityID)
	{
		glm::vec3 lineVertices[4];
		for (usize i = 0; i < 4; i++)
			lineVertices[i] = transform * Renderer2DData::QuadVertexPositions[i];

		DrawLine(lineVertices[0], lineVertices[1], colour, entityID);
		DrawLine(lineVertices[1], lineVertices[2], colour, entityID);
		DrawLine(lineVertices[2], lineVertices[3], colour, entityID);
		DrawLine(lineVertices[3], lineVertices[0], colour, entityID);
	}

	void Renderer2D::DrawCircle(const glm::mat4& transform, const glm::vec4& colour, f32 thickness, i32 entityID)
	{
		if (sRenderData->circleIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		for (usize i = 0; i < 4; i++)
		{
			sRenderData->circleVertexBufferPtr->worldPosition = transform * Renderer2DData::QuadVertexPositions[i];
			sRenderData->circleVertexBufferPtr->thickness = thickness;
			sRenderData->circleVertexBufferPtr->localPosition = Renderer2DData::QuadVertexPositions[i] * 2.0f;
			sRenderData->circleVertexBufferPtr->colour = colour;
			sRenderData->circleVertexBufferPtr->entityID = entityID;
			sRenderData->circleVertexBufferPtr++;
		}

		sRenderData->circleIndexCount += 6;

		sRenderData->stats.quadCount++;
	}

	void Renderer2D::StartBatch()
	{
		sRenderData->quadIndexCount = 0;
		sRenderData->quadVertexBufferPtr = sRenderData->quadVertexBufferBase;

		sRenderData->circleIndexCount = 0;
		sRenderData->circleVertexBufferPtr = sRenderData->circleVertexBufferBase;

		sRenderData->lineVertexCount = 0;
		sRenderData->lineVertexBufferPtr = sRenderData->lineVertexBufferBase;

		sRenderData->textureSlotIndex = 1;
	}

	void Renderer2D::Flush()
	{
		// Quads
		if (sRenderData->quadIndexCount)
		{
			u32 quadDataSize = (u32)((u8*)sRenderData->quadVertexBufferPtr - (u8*)sRenderData->quadVertexBufferBase);
			sRenderData->quadVertexBuffer->SetData(sRenderData->quadVertexBufferBase, quadDataSize);

			for (u32 i = 0; i < sRenderData->textureSlotIndex; i++)
				sRenderData->textureSlots[i]->BindTexture(i);

			sRenderData->quadShader->Bind();
			Renderer::DrawIndexed(sRenderData->quadVertexArray, sRenderData->quadIndexCount);
			sRenderData->stats.drawCalls++;
		}

		// Circles
		if (sRenderData->circleIndexCount)
		{
			u32 circleDataSize = (u32)((u8*)sRenderData->circleVertexBufferPtr - (u8*)sRenderData->circleVertexBufferBase);
			sRenderData->circleVertexBuffer->SetData(sRenderData->circleVertexBufferBase, circleDataSize);

			sRenderData->circleShader->Bind();
			Renderer::DrawIndexed(sRenderData->circleVertexArray, sRenderData->circleIndexCount);
			sRenderData->stats.drawCalls++;
		}

		// Lines
		if (sRenderData->lineVertexCount)
		{
			u32 lineDataSize = (u32)((u8*)sRenderData->lineVertexBufferPtr - (u8*)sRenderData->lineVertexBufferBase);
			sRenderData->lineVertexBuffer->SetData(sRenderData->lineVertexBufferBase, lineDataSize);

			sRenderData->lineShader->Bind();
			Renderer::SetLineWidth(sRenderData->lineWidth);
			Renderer::DrawLines(sRenderData->lineVertexArray, sRenderData->lineVertexCount);
			sRenderData->stats.drawCalls++;
		}
	}

	void Renderer2D::NextBatch()
	{
		Flush();
		StartBatch();
	}

	void Renderer2D::ResetStats()
	{
		memset(&sRenderData->stats, 0, sizeof(RenderStatistics));
	}

	const RenderStatistics& Renderer2D::GetStats()
	{
		return sRenderData->stats;
	}

	void Renderer2D::DrawQuad(const glm::mat4& trans, RenderType type, AssetHandle handle, const glm::vec4 colour, f32 tilingFactor, i32 entityID)
	{
		if (!handle || type == RenderType::None)
		{
			DrawQuad(trans, colour, entityID);
			return;
		}

		switch (type)
		{
		case RenderType::Texture:
		{
			Ref<Texture2D> tex = AssetManager::GetAsset<Texture2D>(handle);
			if (!tex)
				return;

			DrawQuad(trans, tex, tilingFactor, colour, entityID);
			break;
		}
		case RenderType::SubTexture:
		{
			Ref<SubTexture2D> subtex = AssetManager::GetAsset<SubTexture2D>(handle);
			if (!subtex)
				return;

			DrawQuad(trans, subtex, tilingFactor, colour, entityID);
			break;
		}
		case RenderType::Tilemap:
		{
			Ref<Tilemap> tilemap = AssetManager::GetAsset<Tilemap>(handle);
			if (!tilemap)
				return;

			DrawQuad(trans, tilemap, tilingFactor, colour, entityID);
			break;
		}
		}
	}
}
