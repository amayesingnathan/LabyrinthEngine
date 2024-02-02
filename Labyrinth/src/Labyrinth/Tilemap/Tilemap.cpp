#include "Lpch.h"
#include "Tilemap.h"

namespace Laby {

	Tilemap::Tilemap(const std::string& name, u32 width, u32 height)
		: mName(name), mTexture(width, height), mBehaviour(width, height)
	{
	}

	TileBehaviourData Tilemap::GetTileBehaviour(const GridPosition& pos) const
	{
		if (!pos.Valid())
			return {};

		return mBehaviour(pos);
	}

	Ref<Tilemap> TilemapUtils::Clone(Ref<Tilemap> other)
	{
		Ref<Tilemap> copy = Ref<Tilemap>::Create(other->mName, other->GetWidth(), other->GetHeight());

		for (const auto& sheetData : other->mTexture.GetSheets())
			copy->AddSheet(sheetData.sheet->handle, sheetData.startIndex);
		for (const auto& layer : other->mTexture.mLayers)
			copy->AddLayer(layer);

		copy->SetBehaviour(other->mBehaviour);
		copy->handle = other->handle;

		return copy;
	}
}