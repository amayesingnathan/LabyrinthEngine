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
		
		static void EnableDepth()
		{
			sRendererAPI->enableDepth();
		}

		static void DisableDepth()
		{
			sRendererAPI->disableDepth();
		}

		static void EnableBlend()
		{
			sRendererAPI->enableBlend();
		}

		static void DisableBlend()
		{
			sRendererAPI->disableBlend();
		}

		static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t count = 0)
		{
			sRendererAPI->drawIndexed(vertexArray, count);
		}

	private:
		static Single<RendererAPI> sRendererAPI;
	};

}