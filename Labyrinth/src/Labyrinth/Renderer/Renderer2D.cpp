#include "Lpch.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"

#include "Platforms/OpenGL/OpenGLShader.h"

namespace Labyrinth {

	struct Renderer2DStorage
	{
		Ref<VertexArray> quadVertexArray;
		Ref<Shader> flatColourShader;
	};

	static Renderer2DStorage* sData;

	void Renderer2D::Init()
	{
		sData = new Renderer2DStorage();
		sData->quadVertexArray = VertexArray::Create();

		float squareVertices[3 * 4] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.5f,  0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f
		};

		Ref<VertexBuffer> squareVB;
		squareVB = VertexBuffer::Create(squareVertices, sizeof(squareVertices));
		squareVB->setLayout({
			{ ShaderDataType::Float3, "aPosition" },
			});
		sData->quadVertexArray->addVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		Ref<IndexBuffer> squareIB;
		squareIB = IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
		sData->quadVertexArray->setIndexBuffer(squareIB);

		sData->flatColourShader = Shader::Create("assets/shaders/flatcolor.glsl");
	}

	void Renderer2D::Shutdown()
	{
		delete sData;
	}

	void Renderer2D::BeginState(const OrthographicCamera& camera)
	{
		std::dynamic_pointer_cast<OpenGLShader>(sData->flatColourShader)->bind();
		std::dynamic_pointer_cast<OpenGLShader>(sData->flatColourShader)->uploadUniformMat4("uViewProjection", camera.getViewProjectionMatrix());
		std::dynamic_pointer_cast<OpenGLShader>(sData->flatColourShader)->uploadUniformMat4("uTransform", glm::mat4(1.0f));
	}

	void Renderer2D::EndState()
	{
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& colour)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, colour);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& colour)
	{
		sData->flatColourShader->bind();
		sData->flatColourShader->setFloat4("uColor", colour);

		sData->quadVertexArray->bind();
		RenderCommand::DrawIndexed(sData->quadVertexArray);
	}

}