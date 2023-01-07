#pragma once

#include <Labyrinth/Editor/IEditorModal.h>

namespace Laby {

	struct MapSpec;

	class NewMapModal : public IEditorModal
	{
	public:
		NewMapModal() = default;

		void onImGuiRender() override;
		void onCustomButtonRender(bool& open) override;
		void onComplete() override;

	private:
		std::string mMapName;
		usize mMapWidth = 0, mMapHeight = 0;
	};

}
