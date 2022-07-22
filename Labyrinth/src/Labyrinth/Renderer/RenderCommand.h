#pragma once

#include "RendererAPI.h"

namespace Labyrinth {

	class LAB_API RenderCommand
	{
	public:
		static void Init()
		{
			sRendererAPI->init();
		}

		static void SetViewport(u32 x, u32 y, u32 w, u32 h)
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

		static void DrawIndexed(const Ref<VertexArray>& vertexArray, u32 indexCount = 0)
		{
			sRendererAPI->drawIndexed(vertexArray, indexCount);
		}

		static void DrawLines(const Ref<VertexArray>& vertexArray, u32 vertexCount)
		{
			sRendererAPI->drawLines(vertexArray, vertexCount);
		}

		static void SetLineWidth(f32 width)
		{
			sRendererAPI->setLineWidth(width);
		}

	private:
		static Single<RendererAPI> sRendererAPI;
	};

}