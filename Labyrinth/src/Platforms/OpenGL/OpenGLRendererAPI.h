#pragma once

#include "Labyrinth/Renderer/RendererAPI.h"

namespace Labyrinth {

	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void init();
		virtual void setViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h) override;

		virtual void setClearColor(const glm::vec4& colour) override;
		virtual void clear() override;

		virtual void drawIndexed(const Ref<VertexArray>& vertexArray);
	};

}