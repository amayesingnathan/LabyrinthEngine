#pragma once

#include <Labyrinth/Core/System/Base.h>

#include "VertexArray.h"

namespace Laby {

	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void SetViewport(u32 x, u32 y, u32 w, u32 h);
		static void SetClearColor(const glm::vec4& colour);
		static void Clear();

		static void DrawIndexed(const Ref<VertexArray>& vertexArray, u32 indexCount = 0);
	};

}