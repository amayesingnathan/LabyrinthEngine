#pragma once

#include "RenderCommand.h"

namespace Labyrinth {

	class Renderer
	{
	public:
		static void BeginState();
		static void EndState();

		static void Send(const Ref<VertexArray>& vertexArray);

		static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	};

}