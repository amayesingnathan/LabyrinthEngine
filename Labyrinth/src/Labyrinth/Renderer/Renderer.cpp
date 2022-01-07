#include "Lpch.h"
#include "Renderer.h"

namespace Labyrinth {

	Renderer::SceneData* Renderer::sSceneData = new Renderer::SceneData;

	void Renderer::BeginState(OrthographicCamera& camera)
	{
		sSceneData->ViewProjectionMatrix = camera.getViewProjectionMatrix();
	}

	void Renderer::EndState()
	{
	}

	void Renderer::Send(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray)
	{
		shader->bind();
		shader->uploadUniformMat4("uViewProjection", sSceneData->ViewProjectionMatrix);

		vertexArray->bind();
		RenderCommand::DrawIndexed(vertexArray);
	}

}