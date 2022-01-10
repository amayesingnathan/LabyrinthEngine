#include <Labyrinth.h>
#include "Labyrinth/Core/EntryPoint.h"

#include "EditorLayer.h"

namespace Labyrinth {

	class EnigmaEditor : public Labyrinth::Application
	{
	public:
		EnigmaEditor()
		{
			pushLayer(new EditorLayer());
		}

		~EnigmaEditor()
		{
		}
	};

	Application* Labyrinth::CreateApplication()
	{
		return new EnigmaEditor();
	}

}
