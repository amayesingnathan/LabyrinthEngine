#pragma once

#include <Labyrinth/Core/Timestep.h>
#include <Labyrinth/Events/Event.h>
#include <Labyrinth/Project/Project.h>

namespace Laby {

	class IEditorPanel : public RefCounted
	{
	public:
		virtual ~IEditorPanel() {}

		virtual void onUpdate(Timestep ts) {};
		virtual void onImGuiRender() = 0;
		virtual void onEvent(Event& e) {}
		virtual void onProjectChange(const Ref<Project>& project) {}
		virtual void onSelectionChange() {}
	};

	template<typename T>
	concept IsEditorPanel = DerivedFrom<IEditorPanel, T>;
}