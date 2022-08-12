#pragma once

#include "Labyrinth/Core/System/Base.h"
#include "Labyrinth/Core/UUID.h"
#include "Labyrinth/Events/Event.h"
#include "Labyrinth/Project/Project.h"

namespace Labyrinth {

	class EditorPanel : public RefCounted
	{
	public:
		virtual ~EditorPanel() {}

		virtual void onUpdate() {};
		virtual void onImGuiRender() = 0;
		virtual void onEvent(Event& e) {}
		virtual void onProjectChange(const Ref<Project>& project) {}
		virtual void onSelectionChange() {}
	};
}