#pragma once

namespace Labyrinth {

	class GraphicsContext
	{
	public:
		virtual ~GraphicsContext() = default;

		virtual void init() = 0;
		virtual void swapBuffers() = 0;
		
		static Single<GraphicsContext> Create(void* window);
	};

}