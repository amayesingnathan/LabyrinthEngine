#pragma once

#include "MapLayer.h"

#include <Labyrinth/Assets/AssetGroup.h>
#include <Labyrinth/Core/System/Base.h>
#include <Labyrinth/Renderer/Framebuffer.h>

namespace Labyrinth {

	struct SheetData
	{
		size_t firstID;
		Ref<Texture2DSheet> sheet;

		SheetData(size_t id, const Ref<Texture2DSheet>& _sheet) : firstID(id), sheet(_sheet) {}

		bool operator <(const SheetData& other) const
		{
			return firstID < other.firstID;
		}
	};

	class Tilemap
	{
	private:
		Tilemap() = default;

		const Ref<Texture2DSheet>& GetSheet(size_t tileID);

	public:
		Tilemap(const std::string& name);

		// Re-renders the internal framebuffer object
		const Ref<Framebuffer>& genTex();
		const Ref<Framebuffer>& getTex() const { return mTexture; }

		static Ref<Tilemap> Create(const std::string& name);

	private:
		std::string mMapName;
		std::vector<SheetData> mSheets;
		std::vector<MapLayer> mLayers;
		Ref<Framebuffer> mTexture;

	};
}