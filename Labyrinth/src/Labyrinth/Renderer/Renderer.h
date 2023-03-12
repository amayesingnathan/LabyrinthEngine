#pragma once

#include <Labyrinth/Core/UUID.h>

#include "VertexArray.h"

namespace Laby {

	class Camera;

	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void NewFrame();
		static void EndFrame();

		// Add a callback to run on completion of a frame when the main thread and render thread are synchronised.
		// Returns a UUID to store in case the callback is tied to the lifetime of an object.
		static UUID AddFrameCallback(Action<>&& callback);
		static void RemoveFrameCallback(UUID id);

		static void Submit(Action<>&& renderWork);

		static void SetViewport(u32 w, u32 h);
		static void SetClearColor(const glm::vec4& colour);
		static void Clear();

		static void SetLineWidth(f32 width);

		static void DrawIndexed(const Ref<VertexArray>& vertexArray, u32 indexCount = 0);
		static void DrawLines(const Ref<VertexArray>& vertexArray, u32 vertexCount);

		static void EnableDepth();
		static void SetDepth(f64 nearVal, f64 farVal);
		static void DisableDepth();
	};

}