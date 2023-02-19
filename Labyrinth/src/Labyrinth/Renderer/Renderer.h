#pragma once

#include <Labyrinth/Core/System/Base.h>

#include "VertexArray.h"

namespace Laby {

	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void SetViewport(u32 w, u32 h);
		static void SetClearColor(const glm::vec4& colour);
		static void Clear();

		static void SetLineWidth(f32 width);

		static void DrawIndexed(const Ref<VertexArray>& vertexArray, u32 indexCount = 0);
		static void DrawLines(const Ref<VertexArray>& vertexArray, u32 vertexCount);

		static void EnableDepth();
		static void DisableDepth();
	};

}