#pragma once

#include "RendererAPI.h"

namespace Labyrinth {

	class RenderCommand
	{
	public:
		static void SetClearColor(const glm::vec4& colour)
		{
			sRendererAPI->setClearColor(colour);
		}

		static void Clear()
		{
			sRendererAPI->clear();
		}

		static void DrawIndexed(const Ref<VertexArray>& vertexArray)
		{
			sRendererAPI->drawIndexed(vertexArray);
		}

	private:
		static RendererAPI* sRendererAPI;
	};

}