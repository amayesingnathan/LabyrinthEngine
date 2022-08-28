#pragma once

#include "Labyrinth/Renderer/RendererAPI.h"

namespace Labyrinth {

	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void init();
		virtual void setViewport(u32 x, u32 y, u32 w, u32 h) override;

		virtual void setClearColor(const glm::vec4& colour) override;
		virtual void clear() override;

		virtual void drawIndexed(const Ref<VertexArray>& vertexArray, u32 indexCount = 0) override;
		virtual void drawLines(const Ref<VertexArray>& vertexArray, u32 vertexCount) override;

		virtual void setLineWidth(f32 width) override;

		virtual void enableDepth() override;
		virtual void disableDepth() override;
		virtual void enableBlend() override;
		virtual void disableBlend() override;
	};

}