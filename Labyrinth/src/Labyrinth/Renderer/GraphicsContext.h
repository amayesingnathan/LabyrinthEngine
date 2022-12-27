#pragma once

#include <Labyrinth/Core/System/Ref.h>

namespace Laby {

	class GraphicsContext : public RefCounted
	{
	public:
		virtual ~GraphicsContext() = default;

		virtual void init() = 0;
		virtual void swapBuffers() = 0;

		static Single<GraphicsContext> Create(void* window);
	};

}