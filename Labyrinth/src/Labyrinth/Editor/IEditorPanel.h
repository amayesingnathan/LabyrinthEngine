#pragma once

#include <Labyrinth/Core/Timestep.h>
#include <Labyrinth/Events/IEventListener.h>
#include <Labyrinth/Project/Project.h>

namespace Laby {

	class IEditorPanel : public RefCounted, public IEventListener
	{
	public:
		virtual ~IEditorPanel() {}

		virtual void onUpdate(Timestep ts) {};
		virtual void onImGuiRender() = 0;
		virtual void onProjectChange() {}
		virtual void onSelectionChange() {}

		virtual void onEvent(Event& e) override {}
		LISTENING_EVENTS(None)
	};

	template<typename T>
	concept IsEditorPanel = DerivedFrom<IEditorPanel, T>;
}