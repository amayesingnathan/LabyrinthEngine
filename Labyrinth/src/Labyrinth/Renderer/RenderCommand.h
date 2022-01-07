#pragma once

#include "RendererAPI.h"

namespace Labyrinth {

	class RenderCommand
	{
	public:
		inline static void SetClearColor(const glm::vec4& colour)
		{
			sRendererAPI->setClearColor(colour);
		}

		inline static void Clear()
		{
			sRendererAPI->clear();
		}

		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray)
		{
			sRendererAPI->drawIndexed(vertexArray);
		}

	private:
		static RendererAPI* sRendererAPI;
	};

}