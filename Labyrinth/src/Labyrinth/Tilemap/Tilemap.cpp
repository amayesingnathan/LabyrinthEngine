#include "Lpch.h"
#include "Tilemap.h"

namespace Laby {

	Tilemap::Tilemap(const std::string& name, u32 width, u32 height)
		: mName(name), mTexture(width, height), mBehaviour(width, height)
	{
	}

	TileBehaviourData Tilemap::getTileBehaviour(const GridPosition& pos) const
	{
		if (!pos.valid())
			return {};

		return mBehaviour(pos);
	}

	Ref<Tilemap> TilemapUtils::Clone(Ref<Tilemap> other)
	{
		Ref<Tilemap> copy = Ref<Tilemap>::Create(other->mName, other->getWidth(), other->getHeight());

		for (const auto& layer : other->mTexture.mLayers)
			copy->addLayer(layer);
		for (const auto& sheetData : other->mTexture.getSheets())
			copy->addSheet(sheetData.sheet->handle, sheetData.startIndex);

		copy->setBehaviour(other->mBehaviour);
		copy->handle = other->handle;

		return copy;
	}
}