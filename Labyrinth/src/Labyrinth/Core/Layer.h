#pragma once

#include "Labyrinth/Core/System/Base.h"
#include "Labyrinth/Core/Timestep.h"
#include "Labyrinth/Events/IEventListener.h"

namespace Laby {

	class Layer : public IEventListener
	{
	public:
		Layer() = default;
		virtual ~Layer() = default;

		virtual void onAttach() {}
		virtual void onDetach() {}
		virtual void onUpdate(Timestep ts) {}
		virtual void onImGuiRender() {}
	};

}
