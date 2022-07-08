#pragma once

#include <Labyrinth.h>

#include <imgui/imgui.h>
#include <glm/glm.hpp>

namespace Labyrinth {

	struct MapSpec;

	class MapSpecModal : public ModalWindow
	{
	public:
		MapSpecModal(Entity& data);

		void display() override;

	private:
		Entity& mEntity;
		std::string mMapName;
		uint8_t mLayer = 0;
	};

}
