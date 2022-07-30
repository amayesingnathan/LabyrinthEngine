#include "Lpch.h"
#include "Renderer2D.h"

#include "Labyrinth/Renderer/VertexArray.h"
#include "Labyrinth/Renderer/Shader.h"
#include "Labyrinth/Renderer/UniformBuffer.h"
#include "Labyrinth/Renderer/RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Labyrinth {

	struct DisplayVertex
	{
		glm::vec3 position;
		glm::vec2 texCoord;
	};

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

	struct CircleVertex
	{
		glm::vec3 worldPosition;
		f32 thickness;
		glm::vec2 localPosition;
		glm::vec4 colour;

		// Editor-only
		i32 entityID;
	};

	struct LineVertex
	{
		glm::vec3 position;
		glm::vec4 colour;

		// Editor-only
		i32 entityID;
	};

	struct Renderer2DData
	{
		static const u32 MaxQuads = 20000;
		static const u32 MaxVertices = MaxQuads * 4;
		static const u32 MaxIndices = MaxQuads * 6;
		static const u32 MaxTextureSlots = 32; //ToDo: RenderCaps

		Ref<VertexArray> displayVertexArray;
		Ref<VertexBuffer> displayVertexBuffer;
		Ref<Shader> displayShader;

		DisplayVertex* displayVertexBufferBase = nullptr;
		DisplayVertex* displayVertexBufferPtr = nullptr;

		Ref<VertexArray> quadVertexArray;
		Ref<VertexBuffer> quadVertexBuffer;
		Ref<Shader> quadShader;
		Ref<Texture2D> whiteTexture;

		u32 quadIndexCount = 0;
		QuadVertex* quadVertexBufferBase = nullptr;
		QuadVertex* quadVertexBufferPtr = nullptr;

		Ref<VertexArray> circleVertexArray;
		Ref<VertexBuffer> circleVertexBuffer;
		Ref<Shader> circleShader;

		u32 circleIndexCount = 0;
		CircleVertex* circleVertexBufferBase = nullptr;
		CircleVertex* circleVertexBufferPtr = nullptr;

		Ref<VertexArray> lineVertexArray;
		Ref<VertexBuffer> lineVertexBuffer;
		Ref<Shader> lineShader;

		u32 lineVertexCount = 0;
		LineVertex* lineVertexBufferBase = nullptr;
		LineVertex* lineVertexBufferPtr = nullptr;

		f32 lineWidth = 1.0f;

		std::array<Ref<Texture2D>, MaxTextureSlots> textureSlots;
		u32 textureSlotIndex;

		glm::vec4 displayVertexPositions[4];
		glm::vec4 quadVertexPositions[4]; 

		Renderer2D::Statistics stats;

		struct CameraData { glm::mat4 viewProjection; };
		CameraData cameraBuffer;
		Ref<UniformBuffer> cameraUniformBuffer;
	};

	static Renderer2DData sData;

	void Renderer2D::Init()
	{
		LAB_PROFILE_FUNCTION();

		// Framebuffer
		{
			sData.displayVertexPositions[0] = { -1.0f, -1.0f, 0.0f, 1.0f };
			sData.displayVertexPositions[1] = { 1.0f, -1.0f, 0.0f, 1.0f };
			sData.displayVertexPositions[2] = { 1.0f,  1.0f, 0.0f, 1.0f };
			sData.displayVertexPositions[3] = { -1.0f,  1.0f, 0.0f, 1.0f };

			sData.displayVertexArray = VertexArray::Create();

			sData.displayVertexBuffer = VertexBuffer::Create(4 * sizeof(DisplayVertex));
			sData.displayVertexBuffer->setLayout({
				{ ShaderDataType::Float3, "aPosition" },
				{ ShaderDataType::Float2, "aTexCoord" }
				});
			sData.displayVertexArray->addVertexBuffer(sData.displayVertexBuffer);

			sData.displayVertexBufferBase = new DisplayVertex[4];
			sData.displayVertexBufferPtr = sData.displayVertexBufferBase;

			u32* displayIndices = new u32[6];

			displayIndices[0] = 0;
			displayIndices[1] = 1;
			displayIndices[2] = 2;
			displayIndices[3] = 2;
			displayIndices[4] = 3;
			displayIndices[5] = 0;

			Ref<IndexBuffer> displayIB = IndexBuffer::Create(displayIndices, 6);

			sData.displayVertexArray->setIndexBuffer(displayIB);
			delete[] displayIndices;
		}

		// Quads
		{
			sData.quadVertexArray = VertexArray::Create();

			sData.quadVertexBuffer = VertexBuffer::Create(sData.MaxVertices * sizeof(QuadVertex));
			sData.quadVertexBuffer->setLayout({
				{ ShaderDataType::Float3, "aPosition"	  },
				{ ShaderDataType::Float4, "aColour"		  },
				{ ShaderDataType::Float2, "aTexCoord"	  },
				{ ShaderDataType::Float,  "aTexIndex"	  },
				{ ShaderDataType::Float,  "aTilingFactor" },
				{ ShaderDataType::Int,    "aEntityID"	  }
				});

			sData.quadVertexArray->addVertexBuffer(sData.quadVertexBuffer);

			sData.quadVertexBufferBase = new QuadVertex[sData.MaxVertices];

			u32* quadIndices = new u32[sData.MaxIndices];

			u32 offset = 0;
			for (u32 i = 0; i < sData.MaxIndices; i += 6)
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
			delete[] quadIndices;
		}

		// Circles
		{
			sData.circleVertexArray = VertexArray::Create();

			sData.circleVertexBuffer = VertexBuffer::Create(sData.MaxVertices * sizeof(QuadVertex));
			sData.circleVertexBuffer->setLayout({
				{ ShaderDataType::Float3, "aWorldPosition" },
				{ ShaderDataType::Float,  "aThickness"     },
				{ ShaderDataType::Float2, "aLocalPosition" },
				{ ShaderDataType::Float4, "aColour"        },
				{ ShaderDataType::Int,    "aEntityID"      }
				});

			sData.circleVertexArray->addVertexBuffer(sData.circleVertexBuffer);

			sData.circleVertexBufferBase = new CircleVertex[sData.MaxVertices];
			sData.circleVertexArray->setIndexBuffer(sData.quadVertexArray->getIndexBuffer()); // Reuse quad index buffer
		}

		// Lines
		{
			sData.lineVertexArray = VertexArray::Create();

			sData.lineVertexBuffer = VertexBuffer::Create(sData.MaxVertices * sizeof(QuadVertex));
			sData.lineVertexBuffer->setLayout({
				{ ShaderDataType::Float3, "aPosition" },
				{ ShaderDataType::Float4, "aColour"   },
				{ ShaderDataType::Int,    "aEntityID" }
				});

			sData.lineVertexArray->addVertexBuffer(sData.lineVertexBuffer);
			sData.lineVertexBufferBase = new LineVertex[sData.MaxVertices];
		}

		// White Texture
		sData.whiteTexture = Texture2D::Create(1, 1);
		u32 whiteTextureData = 0xffffffff;
		sData.whiteTexture->setData(&whiteTextureData, sizeof(u32));

		sData.quadShader = Shader::Create("assets/shaders/Renderer2DQuad.glsl");
		sData.circleShader = Shader::Create("assets/shaders/Renderer2DCircle.glsl");
		sData.lineShader = Shader::Create("assets/shaders/Renderer2DLine.glsl");
		sData.displayShader = Shader::Create("assets/shaders/Framebuffer.glsl");

		sData.textureSlots[0] = sData.whiteTexture;

		sData.quadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		sData.quadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		sData.quadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		sData.quadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		sData.cameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer2DData::CameraData), 0);
	}

	void Renderer2D::Shutdown()
	{
		LAB_PROFILE_FUNCTION();

		delete[] sData.quadVertexBufferBase;
		delete[] sData.circleVertexBufferBase;
		delete[] sData.lineVertexBufferBase;
		delete[] sData.displayVertexBufferBase;
	}

	void Renderer2D::BeginState()
	{
		LAB_PROFILE_FUNCTION();

		sData.cameraBuffer.viewProjection = glm::mat4(1.0f);
		sData.cameraUniformBuffer->setData(&sData.cameraBuffer, sizeof(Renderer2DData::CameraData));

		StartBatch();
	}

	void Renderer2D::BeginState(const Camera& camera, const glm::mat4 transform)
	{
		LAB_PROFILE_FUNCTION();

		sData.cameraBuffer.viewProjection = camera.getProjection() * glm::inverse(transform);
		sData.cameraUniformBuffer->setData(&sData.cameraBuffer, sizeof(Renderer2DData::CameraData));

		StartBatch();
	}

	void Renderer2D::BeginState(const OrthographicCamera& camera)
	{
		LAB_PROFILE_FUNCTION();

		sData.cameraBuffer.viewProjection = camera.getProjectionMatrix();
		sData.cameraUniformBuffer->setData(&sData.cameraBuffer, sizeof(Renderer2DData::CameraData));

		StartBatch();
	}

	void Renderer2D::BeginState(const EditorCamera& camera)
	{
		LAB_PROFILE_FUNCTION();

		sData.cameraBuffer.viewProjection = camera.getViewProjection();
		sData.cameraUniformBuffer->setData(&sData.cameraBuffer, sizeof(Renderer2DData::CameraData));

		StartBatch();
	}

	void Renderer2D::EndState()
	{
		LAB_PROFILE_FUNCTION();

		Flush();
	}
	
	void Renderer2D::StartBatch()
	{
		sData.quadIndexCount = 0;
		sData.quadVertexBufferPtr = sData.quadVertexBufferBase;

		sData.circleIndexCount = 0;
		sData.circleVertexBufferPtr = sData.circleVertexBufferBase;

		sData.lineVertexCount = 0;
		sData.lineVertexBufferPtr = sData.lineVertexBufferBase;

		sData.textureSlotIndex = 1;
	}

	void Renderer2D::Flush()
	{
		// Quads
		if (sData.quadIndexCount) 
		{
			u32 quadDataSize = (u32)((u8*)sData.quadVertexBufferPtr - (u8*)sData.quadVertexBufferBase);
			sData.quadVertexBuffer->setData(sData.quadVertexBufferBase, quadDataSize);

			for (u32 i = 0; i < sData.textureSlotIndex; i++)
				sData.textureSlots[i]->bind(i);

			sData.quadShader->bind();
			RenderCommand::DrawIndexed(sData.quadVertexArray, sData.quadIndexCount);
			sData.stats.drawCalls++;
		}

		// Circles
		if (sData.circleIndexCount)
		{
			u32 circleDataSize = (u32)((u8*)sData.circleVertexBufferPtr - (u8*)sData.circleVertexBufferBase);
			sData.circleVertexBuffer->setData(sData.circleVertexBufferBase, circleDataSize);

			sData.circleShader->bind();
			RenderCommand::DrawIndexed(sData.circleVertexArray, sData.circleIndexCount);
			sData.stats.drawCalls++;
		}

		// Lines
		if (sData.lineVertexCount)
		{
			u32 lineDataSize = (u32)((u8*)sData.lineVertexBufferPtr - (u8*)sData.lineVertexBufferBase);
			sData.lineVertexBuffer->setData(sData.lineVertexBufferBase, lineDataSize);

			sData.lineShader->bind();
			RenderCommand::SetLineWidth(sData.lineWidth);
			RenderCommand::DrawLines(sData.lineVertexArray, sData.lineVertexCount);
			sData.stats.drawCalls++;
		}
	}

	void Renderer2D::NextBatch()
	{
		Flush();
		StartBatch();
	}

	void Renderer2D::DrawFramebuffer(Ref<Framebuffer> framebuffer)
	{
		LAB_PROFILE_FUNCTION();

		framebuffer->unbind(); // Rendering to framebuffer complete so switch to default;
		RenderCommand::Clear();
		RenderCommand::DisableDepth();

		const glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		for (usize i = 0; i < 4; i++)
		{
			sData.displayVertexBufferPtr->position = sData.displayVertexPositions[i];
			sData.displayVertexBufferPtr->texCoord = textureCoords[i];
			sData.displayVertexBufferPtr++;
		}

		u32 dataSize = (u32)((u8*)sData.displayVertexBufferPtr - (u8*)sData.displayVertexBufferBase);
		sData.displayVertexBuffer->setData(sData.displayVertexBufferBase, dataSize);

		sData.displayVertexArray->bind();
		sData.displayShader->bind();

		framebuffer->bindColourAttachment();

		RenderCommand::DrawIndexed(sData.displayVertexArray, 6);
		sData.stats.drawCalls++;

		// Reset back to framebuffer rendering.
		RenderCommand::EnableDepth();
		sData.displayVertexBufferPtr = sData.displayVertexBufferBase;
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

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, f32 tilingFactor, const glm::vec4& tintColour, const glm::vec2* textureCoords)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, tilingFactor, tintColour, textureCoords);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, f32 tilingFactor, const glm::vec4& tintColour, const glm::vec2* textureCoords)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tilingFactor, tintColour, textureCoords);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<SubTexture2D>& subtexture, f32 tilingFactor, const glm::vec4& tintColour)
	{
		DrawQuad(position, size, subtexture->getBaseTex(), tilingFactor, tintColour, subtexture->getTexCoords());
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<SubTexture2D>& subtexture, f32 tilingFactor, const glm::vec4& tintColour)
	{
		DrawQuad(position, size, subtexture->getBaseTex(), tilingFactor, tintColour, subtexture->getTexCoords());
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& colour, i32 entityID)
	{
		LAB_PROFILE_FUNCTION();

		constexpr usize quadVertexCount = 4;
		const f32 textureIndex = 0.0f; // White Texture
		constexpr glm::vec2 textureCoords[4] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		const f32 tilingFactor = 1.0f;

		if (sData.quadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		for (usize i = 0; i < quadVertexCount; i++)
		{
			sData.quadVertexBufferPtr->position = transform * sData.quadVertexPositions[i];
			sData.quadVertexBufferPtr->colour = colour;
			sData.quadVertexBufferPtr->texCoord = textureCoords[i];
			sData.quadVertexBufferPtr->texIndex = textureIndex;
			sData.quadVertexBufferPtr->tilingFactor = tilingFactor;
			sData.quadVertexBufferPtr->entityID = entityID;
			sData.quadVertexBufferPtr++;
		}

		sData.quadIndexCount += 6;

		sData.stats.quadCount++;
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, f32 tilingFactor, const glm::vec4& tintColour, const glm::vec2* textureCoords, i32 entityID)
	{
		LAB_PROFILE_FUNCTION();

		constexpr usize quadVertexCount = 4;
		const glm::vec2 defTextureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		if (!textureCoords)
			textureCoords = defTextureCoords;

		if (sData.quadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		f32 textureIndex = 0.0f;
		for (u32 i = 1; i < sData.textureSlotIndex; i++)
		{
			if (*sData.textureSlots[i] == *texture)
			{
				textureIndex = (f32)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if (sData.textureSlotIndex >= Renderer2DData::MaxTextureSlots)
				NextBatch();

			textureIndex = (f32)sData.textureSlotIndex;
			sData.textureSlots[sData.textureSlotIndex] = texture;
			sData.textureSlotIndex++;
		}

		for (usize i = 0; i < quadVertexCount; i++)
		{
			sData.quadVertexBufferPtr->position = transform * sData.quadVertexPositions[i];
			sData.quadVertexBufferPtr->colour = tintColour;
			sData.quadVertexBufferPtr->texCoord = textureCoords[i];
			sData.quadVertexBufferPtr->texIndex = textureIndex;
			sData.quadVertexBufferPtr->tilingFactor = tilingFactor;
			sData.quadVertexBufferPtr->entityID = entityID;
			sData.quadVertexBufferPtr++;
		}

		sData.quadIndexCount += 6;

		sData.stats.quadCount++;
	}

	void Renderer2D::DrawSprite(const glm::mat4& transform, const SpriteRendererComponent& src, i32 entityID)
	{
		if (src.hasTex()) {
			switch (src.type)
			{
			case SpriteRendererComponent::TexType::Texture:
			{
				DrawQuad(transform, AssetManager::GetAsset<Texture2D>(src.handle),
					src.tilingFactor, src.colour, nullptr, entityID);
				break;
			}
			case SpriteRendererComponent::TexType::SubTexture:
			{
				Ref<SubTexture2D> subtex = AssetManager::GetAsset<SubTexture2D>(src.handle);
				DrawQuad(transform, subtex->getBaseTex(),
					src.tilingFactor, src.colour,
					subtex->getTexCoords(),
					entityID);
				break;
			}
			}
		} else DrawQuad(transform, src.colour, entityID);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, f32 rotation, const glm::vec4& colour)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, colour);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, f32 rotation, const glm::vec4& colour)
	{
		LAB_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, colour);

	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, f32 rotation, const Ref<Texture2D>& texture, f32 tilingFactor, const glm::vec4& tintColour, const glm::vec2* textureCoords)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, tilingFactor, tintColour, textureCoords);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, f32 rotation, const Ref<Texture2D>& texture, f32 tilingFactor, const glm::vec4& tintColour, const glm::vec2* textureCoords)
	{
		LAB_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tilingFactor, tintColour, textureCoords);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, f32 rotation, const Ref<SubTexture2D>& subtexture, f32 tilingFactor, const glm::vec4& tintColour)
	{
		DrawRotatedQuad(position, size, rotation, subtexture->getBaseTex(), tilingFactor, tintColour, subtexture->getTexCoords());
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, f32 rotation, const Ref<SubTexture2D>& subtexture, f32 tilingFactor, const glm::vec4& tintColour)
	{
		DrawRotatedQuad(position, size, rotation, subtexture->getBaseTex(), tilingFactor, tintColour, subtexture->getTexCoords());
	}

	void Renderer2D::DrawCircle(const glm::mat4& transform, const glm::vec4& colour, f32 thickness, i32 entityID)
	{
		LAB_PROFILE_FUNCTION();

		if (sData.circleIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		for (usize i = 0; i < 4; i++)
		{
			sData.circleVertexBufferPtr->worldPosition = transform * sData.quadVertexPositions[i];
			sData.circleVertexBufferPtr->thickness = thickness;
			sData.circleVertexBufferPtr->localPosition = sData.quadVertexPositions[i] * 2.0f;
			sData.circleVertexBufferPtr->colour = colour;
			sData.circleVertexBufferPtr->entityID = entityID;
			sData.circleVertexBufferPtr++;
		}

		sData.circleIndexCount += 6;

		sData.stats.quadCount++;
	}

	void Renderer2D::DrawCircle(const glm::mat4& transform, const CircleRendererComponent& src, i32 entityID)
	{
		DrawCircle(transform, src.colour, src.thickness, entityID);
	}

	void Renderer2D::DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& colour, i32 entityID)
	{
		sData.lineVertexBufferPtr->position = p0;
		sData.lineVertexBufferPtr->colour = colour;
		sData.lineVertexBufferPtr->entityID = entityID;
		sData.lineVertexBufferPtr++;

		sData.lineVertexBufferPtr->position = p1;
		sData.lineVertexBufferPtr->colour = colour;
		sData.lineVertexBufferPtr->entityID = entityID;
		sData.lineVertexBufferPtr++;

		sData.lineVertexCount += 2;
	}

	void Renderer2D::DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& colour, i32 entityID)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawRect(transform, colour, entityID);
	}

	void Renderer2D::DrawRect(const glm::mat4& transform, const glm::vec4& colour, i32 entityID)
	{
		glm::vec3 lineVertices[4];
		for (usize i = 0; i < 4; i++)
			lineVertices[i] = transform * sData.quadVertexPositions[i];

		DrawLine(lineVertices[0], lineVertices[1], colour, entityID);
		DrawLine(lineVertices[1], lineVertices[2], colour, entityID);
		DrawLine(lineVertices[2], lineVertices[3], colour, entityID);
		DrawLine(lineVertices[3], lineVertices[0], colour, entityID);
	}

	f32 Renderer2D::GetLineWidth() { return sData.lineWidth; }

	void Renderer2D::SetLineWidth(f32 width) { sData.lineWidth = width; }

	void Renderer2D::ResetStats()
	{
		memset(&sData.stats, 0, sizeof(Statistics));
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return sData.stats;
	}


}
