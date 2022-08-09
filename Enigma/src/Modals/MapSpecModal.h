#pragma once

#include <Labyrinth/Editor/EditorModal.h>

#include <Labyrinth/Scene/Entity.h>

#include <imgui/imgui.h>
#include <glm/glm.hpp>

namespace Labyrinth {

	struct MapSpec;

	class MapSpecModal : public EditorModal
	{
	public:
		MapSpecModal(Entity& data);

		void onImGuiRender() override;
		void onComplete() override;

		static Ref<MapSpecModal> Create(Entity& data) { return Ref<MapSpecModal>::Create(data); }

	private:
		Entity& mEntity;
		std::string mMapName;
		u8 mLayer = 0;
	};

}
