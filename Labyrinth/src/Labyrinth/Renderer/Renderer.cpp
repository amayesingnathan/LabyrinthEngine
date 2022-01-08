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

	void Renderer::Send(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		shader->bind();
		shader->uploadUniformMat4("uViewProjection", sSceneData->ViewProjectionMatrix);
		shader->uploadUniformMat4("uTransform", transform);

		vertexArray->bind();
		RenderCommand::DrawIndexed(vertexArray);
	}

}