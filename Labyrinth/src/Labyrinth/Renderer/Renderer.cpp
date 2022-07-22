#include "Lpch.h"
#include "Renderer.h"
#include "Renderer2D.h"

namespace Labyrinth {

	Renderer::SceneData* Renderer::sSceneData = new Renderer::SceneData;

	void Renderer::Init()
	{
		LAB_PROFILE_FUNCTION();

		RenderCommand::Init();
		Renderer2D::Init();
	}

	void Renderer::Shutdown()
	{
		Renderer2D::Shutdown();
	}

	void Renderer::OnWindowResize(u32 w, u32 h)
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
		shader->setMat4("uViewProjection", sSceneData->ViewProjectionMatrix);
		shader->setMat4("uTransform", transform);

		vertexArray->bind();
		RenderCommand::DrawIndexed(vertexArray);
	}

}