#pragma once

#include <Labyrinth/Core/Timestep.h>
#include "Labyrinth/Events/Event.h"
#include <Labyrinth/Project/Project.h>

namespace Laby {

	class IEditorPanel : public RefCounted, public IEventListener
	{
	public:
		virtual ~IEditorPanel() {}

		virtual void OnRender() = 0;
		virtual void OnProjectChange() {}
		virtual void OnSelectionChange() {}

		virtual void OnEvent(Event& e) override {}
		LISTENING_EVENTS(None)
	};

	template<typename T>
	concept IsEditorPanel = DerivedFrom<IEditorPanel, T>;
}