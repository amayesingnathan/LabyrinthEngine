#pragma once

#include <Labyrinth/Editor/IEditorModal.h>

namespace Laby {

	class NewMapModal : public IEditorModal
	{
	public:
		NewMapModal() = default;

		void onImGuiRender() override;
		void onCustomButtonRender(bool& open) override;
		void onComplete() override;

	private:
		std::string mMapName;
		u32 mMapWidth = 0, mMapHeight = 0;
	};

}
