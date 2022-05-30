#pragma once

#include <glm/glm.hpp>

#include "VertexArray.h"

namespace Labyrinth {

	class LAB_API RendererAPI
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

		virtual void setViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h) = 0;
		virtual void setClearColor(const glm::vec4& color) = 0;

		virtual void clear() = 0;

		virtual void drawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;

		virtual void enableDepth() {}
		virtual void disableDepth() {}
		virtual void enableBlend() {}
		virtual void disableBlend() {}

		static API GetAPI() { return sAPI; }

	private:
		static API sAPI;
	};

}