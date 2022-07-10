#pragma once

#include "Modal.h"

#include <Labyrinth/Scene/Entity.h>

#include <imgui/imgui.h>
#include <glm/glm.hpp>

namespace Labyrinth {

	struct MapSpec;

	class MapSpecModal : public Modal
	{
	public:
		MapSpecModal(Entity& data);

		void onImGuiRender() override;

		static Ref<MapSpecModal> Create(Entity& data) { return CreateRef<MapSpecModal>(data); }

	private:
		Entity& mEntity;
		std::string mMapName;
		uint8_t mLayer = 0;
	};

}
