#pragma once

#include <Labyrinth/Editor/EditorModal.h>

#include <imgui/imgui.h>
#include <glm/glm.hpp>

namespace Labyrinth {

	struct MapSpec;

	class NewMapModal : public EditorModal
	{
	public:
		NewMapModal() : EditorModal() {}

		void onImGuiRender() override;
		void onCustomButtonRender() override;
		void onComplete() override;

		static Ref<NewMapModal> Create() { return Ref<NewMapModal>::Create(); }

	private:
		std::string mMapName;
		i32 mMapWidth = 0, mMapHeight = 0;
		fs::path mMapPath;
	};

}
