#pragma once

#include "Labyrinth/Core/Base.h"
#include "Labyrinth/Events/Event.h"

namespace Labyrinth {

	class Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void onAttach() {}
		virtual void onDetach() {}
		virtual void onUpdate() {}
		virtual void onEvent(Event& event) {}

#ifdef LAB_DEBUG
		inline const std::string& ogetName() const { return mDebugName; }

	protected:
		std::string mDebugName;
#endif
	};

}
