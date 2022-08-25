#pragma once

#include <glm/glm.hpp>

#include "VertexArray.h"

namespace Labyrinth {

	class RendererAPI
	{
	public:
		enum class API
		{
			None	= 0,
			OpenGL	= 1
		};

	public:
		virtual ~RendererAPI() = default;

		virtual void init() = 0;

		virtual void setViewport(u32 x, u32 y, u32 w, u32 h) = 0;
		virtual void setClearColor(const glm::vec4& color) = 0;

		virtual void clear() = 0;

		virtual void drawIndexed(const Ref<VertexArray>& vertexArray, u32 indexCount = 0) = 0;
		virtual void drawLines(const Ref<VertexArray>& vertexArray, u32 vertexCount) = 0;
		virtual void setLineWidth(f32 width) = 0;

		virtual void enableDepth() {}
		virtual void disableDepth() {}
		virtual void enableBlend() {}
		virtual void disableBlend() {}

		static API GetAPI() { return sAPI; }

	private:
		static API sAPI;
	};

}