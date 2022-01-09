#pragma once

#include "RendererAPI.h"

namespace Labyrinth {

	class RenderCommand
	{
	public:
		static void Init()
		{
			sRendererAPI->init();
		}

		static void SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
		{
			sRendererAPI->setViewport(x, y, w, h);
		}

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