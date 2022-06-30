#pragma once

#include <Labyrinth/Core/System/Base.h>

namespace Labyrinth {

	class IAsset 
	{
	public:
		virtual ~IAsset() {}
	};

	class Texture2D;
	class SubTexture2D;
	class Texture2DSheet;

	template<typename... AssetType>
	struct AssetTypeGroup {};
	using AllAssetTypes = AssetTypeGroup
	<
		Texture2D,
		SubTexture2D,
		Texture2DSheet
	>;

}