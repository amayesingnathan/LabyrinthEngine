#include "Lpch.h"
#include "AssetManager.h"

#include <Labyrinth/Renderer/SubTexture.h>

namespace Labyrinth {

	size_t AssetManager::GetSubTexRefCount(const Ref<IAsset>& asset)
    {
        size_t count = 0;

        // If this is not a sheet then no subtextures to count.
        Ref<Texture2DSheet> isSheet = CastRefToRelative<Texture2DSheet>(asset);
        if (!isSheet) return count;

        for (const auto& [name, tex] : isSheet->getSubTexList())
            count += (tex.use_count() - 1);

        return count;
    }

}