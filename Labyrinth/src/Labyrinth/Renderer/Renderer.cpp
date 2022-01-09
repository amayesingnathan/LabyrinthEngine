#include "Lpch.h"
#include "Renderer.h"

#include "Platforms/OpenGL/OpenGLShader.h"

namespace Labyrinth {

	Renderer::SceneData* Renderer::sSceneData = new Renderer::SceneData;

	void Renderer::Init()
	{
		RenderCommand::Init();
	}

	void Renderer::OnWindowResize(uint32_t w, uint32_t h)
	{
		RenderCommand::SetViewport(0, 0, w, h);
	}

	void Renderer::BeginState(OrthographicCamera& camera)
	{
		sSceneData->ViewProjectionMatrix = camera.getViewProjectionMatrix();
	}

	void Renderer::EndState()
	{
	}

	void Renderer::Send(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		shader->bind();
		std::dynamic_pointer_cast<OpenGLShader>(shader)->uploadUniformMat4("uViewProjection", sSceneData->ViewProjectionMatrix);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->uploadUniformMat4("uTransform", transform);

		vertexArray->bind();
		RenderCommand::DrawIndexed(vertexArray);
	}

}