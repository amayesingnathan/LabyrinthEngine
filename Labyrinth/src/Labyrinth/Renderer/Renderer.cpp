#include "Lpch.h"
#include "Renderer.h"

namespace Labyrinth {

	void Renderer::BeginState()
	{
	}

	void Renderer::EndState()
	{
	}

	void Renderer::Send(const Ref<VertexArray>& vertexArray)
	{
		vertexArray->bind();
		RenderCommand::DrawIndexed(vertexArray);
	}

}