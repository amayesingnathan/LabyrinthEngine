#include "Lpch.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Labyrinth {

	struct Renderer2DStorage
	{
		Ref<VertexArray> quadVertexArray;
		Ref<Shader> textureShader;
		Ref<Texture2D> whiteTexture;
	};

	static Renderer2DStorage* sData;

	void Renderer2D::Init()
	{
		LAB_PROFILE_FUNCTION();

		sData = new Renderer2DStorage();
		sData->quadVertexArray = VertexArray::Create();

		float squareVertices[5 * 4] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};

		Ref<VertexBuffer> squareVB;
		squareVB = VertexBuffer::Create(squareVertices, sizeof(squareVertices));
		squareVB->setLayout({
			{ ShaderDataType::Float3, "aPosition" },
			{ ShaderDataType::Float2, "aTexCoord" },
			});
		sData->quadVertexArray->addVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		Ref<IndexBuffer> squareIB;
		squareIB = IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
		sData->quadVertexArray->setIndexBuffer(squareIB);

		sData->whiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		sData->whiteTexture->setData(&whiteTextureData, sizeof(uint32_t));

		sData->textureShader = Shader::Create("assets/shaders/texture.glsl");
		sData->textureShader->bind();
		sData->textureShader->setInt("uTexture", 0);
	}

	void Renderer2D::Shutdown()
	{
		LAB_PROFILE_FUNCTION();

		delete sData;
	}

	void Renderer2D::BeginState(const OrthographicCamera& camera)
	{
		LAB_PROFILE_FUNCTION();

		sData->textureShader->bind();
		sData->textureShader->setMat4("uViewProjection", camera.getViewProjectionMatrix());
	}

	void Renderer2D::EndState()
	{
		LAB_PROFILE_FUNCTION();

	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& colour)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, colour);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& colour)
	{
		LAB_PROFILE_FUNCTION();

		sData->textureShader->setFloat4("uColor", colour);
		sData->textureShader->setFloat("uScalingFactor", 1.0f);
		sData->whiteTexture->bind();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) 
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		sData->textureShader->setMat4("uTransform", transform);

		sData->quadVertexArray->bind();
		RenderCommand::DrawIndexed(sData->quadVertexArray);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float scale, const glm::vec4& tintColour)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, scale, tintColour);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float scale, const glm::vec4& tintColour)
	{
		LAB_PROFILE_FUNCTION();

		sData->textureShader->setFloat4("uColor", glm::vec4(1.0f));
		sData->textureShader->setFloat("uScalingFactor", 1.0f);
		texture->bind();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) 
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		sData->textureShader->setMat4("uTransform", transform);

		texture->bind();

		sData->quadVertexArray->bind();
		RenderCommand::DrawIndexed(sData->quadVertexArray);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& colour)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, colour);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& colour)
	{
		LAB_PROFILE_FUNCTION();

		sData->textureShader->setFloat4("uColor", colour);
		sData->textureShader->setFloat("uScalingFactor", 1.0f);
		sData->whiteTexture->bind();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		sData->textureShader->setMat4("uTransform", transform);

		sData->quadVertexArray->bind();
		RenderCommand::DrawIndexed(sData->quadVertexArray);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float scale, const glm::vec4& tintColour)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, scale, tintColour);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float scale, const glm::vec4& tintColour)
	{
		LAB_PROFILE_FUNCTION();

		sData->textureShader->setFloat4("uColor", tintColour);
		sData->textureShader->setFloat("uScalingFactor", scale);
		sData->whiteTexture->bind();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		sData->textureShader->setMat4("uTransform", transform);

		sData->quadVertexArray->bind();
		RenderCommand::DrawIndexed(sData->quadVertexArray);
	}

}