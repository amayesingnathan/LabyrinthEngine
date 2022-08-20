#pragma once

#include <Labyrinth/Editor/EditorModal.h>
#include <Labyrinth/Tilemap/Tilemap.h>

#include <imgui/imgui.h>
#include <glm/glm.hpp>

namespace Labyrinth {

	class MapEditModal : public EditorModal
	{
	public:
		MapEditModal(const Ref<Tilemap>& map);

		void onImGuiRender() override;
		void onComplete() override;
		void onCustomButtonRender(bool& open) override {}

		static Ref<MapEditModal> Create(const Ref<Tilemap>& map) { return Ref<MapEditModal>::Create(map); }

	private:
		Ref<Tilemap> mTilemap;
		TexMapLayer mCurrentLayer;

		i32 mMapWidth, mMapHeight;
	};

}
