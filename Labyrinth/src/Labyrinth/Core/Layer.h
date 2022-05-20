#pragma once

#include "Labyrinth/Core/System/Base.h"
#include "Labyrinth/Core/Timestep.h"
#include "Labyrinth/Events/Event.h"

namespace Labyrinth {

	class LAB_API Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer() = default;

		virtual void onAttach() {}
		virtual void onDetach() {}
		virtual void onUpdate(Timestep ts) {}
		virtual void onImGuiRender() {}
		virtual void onEvent(Event& event) {}

#ifdef LAB_DEBUG
		const std::string& getName() const { return mDebugName; }

	protected:
		std::string mDebugName;
#endif
	};

}
