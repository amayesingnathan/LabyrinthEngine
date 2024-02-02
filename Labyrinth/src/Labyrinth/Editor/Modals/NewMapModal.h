#pragma once

#include <Labyrinth/Editor/IEditorModal.h>

namespace Laby {

	class NewMapModal : public IEditorModal
	{
	public:
		NewMapModal() = default;

		void OnRender() override;
		void OnCustomButtonRender(bool& open) override;
		void OnComplete() override;

	private:
		std::string mMapName;
		u32 mMapWidth = 0, mMapHeight = 0;
	};

}
