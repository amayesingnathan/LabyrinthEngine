#pragma once

#include "Labyrinth/Renderer/RendererAPI.h"

namespace Labyrinth {

	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void setClearColor(const glm::vec4& colour) override;
		virtual void clear() override;

		virtual void drawIndexed(const Ref<VertexArray>& vertexArray);
	};

}