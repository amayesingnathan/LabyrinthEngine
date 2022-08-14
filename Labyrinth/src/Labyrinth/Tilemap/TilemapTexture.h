#pragma once

#include "TexMapLayer.h"

#include <Labyrinth/Core/System/Base.h>
#include <Labyrinth/IO/Filesystem.h>
#include <Labyrinth/Renderer/SubTexture.h>

namespace Labyrinth {

	struct SheetData
	{
		usize firstID;
		AssetHandle sheet;

		SheetData(usize id, AssetHandle tex) : firstID(id), sheet(tex) {}

		bool operator <(const SheetData& other) const
		{
			return firstID < other.firstID;
		}
	};

	class TilemapTexture : public RefCounted
	{
	public:
		TilemapTexture(usize width, usize height);
		TilemapTexture(const fs::path& path, usize width, usize height);

		const Ref<Texture2D>& getTex() const { return mTexture; }

		static Ref<TilemapTexture> Create(usize width, usize height) { return Ref<TilemapTexture>::Create(width, height); }
		static Ref<TilemapTexture> Create(const fs::path& path, usize width, usize height) { return Ref<TilemapTexture>::Create(path, width, height); }

	private:
		void RegenTexture();
		Ref<Texture2DSheet> GetSheet(usize tileID) const;

	private:
		usize mWidth = 0, mHeight = 0;
		std::vector<SheetData> mSheets;
		std::vector<TexMapLayer> mLayers;
		Ref<Texture2D> mTexture = nullptr;

		friend class Tilemap;
	};

}