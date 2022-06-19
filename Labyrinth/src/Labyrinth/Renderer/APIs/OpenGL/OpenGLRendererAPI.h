#pragma once

#include "Labyrinth/Renderer/RendererAPI.h"

namespace Labyrinth {

	class LAB_API OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void init();
		virtual void setViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h) override;

		virtual void setClearColor(const glm::vec4& colour) override;
		virtual void clear() override;

		virtual void drawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) override;
		virtual void drawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) override;

		virtual void setLineWidth(float width) override;

		virtual void enableDepth() override;
		virtual void disableDepth() override;
		virtual void enableBlend() override;
		virtual void disableBlend() override;
	};

}