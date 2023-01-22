#include "Lpch.h"
#include "Tilemap.h"

namespace Laby {

	Tilemap::Tilemap(const std::string& name, usize width, usize height)
		: mName(name), mTexture(width, height), mBehaviour(width, height)
	{
	}

	Ref<Tilemap> TilemapUtils::Clone(Ref<Tilemap> other)
	{
		Ref<Tilemap> copy = Ref<Tilemap>::Create(other->mName, other->getWidth(), other->getHeight());

		for (const auto& layer : other->mTexture.mLayers)
			copy->addLayer(layer);
		for (const auto& sheet : other->mTexture.getSheets())
			copy->addSheet(sheet);

		copy->setBehaviour(other->mBehaviour);
		copy->handle = other->handle;

		return copy;
	}
}