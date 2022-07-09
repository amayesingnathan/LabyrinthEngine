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

		SheetData(size_t id, const Ref<Texture2DSheet>& _sheet) : firstID(id), sheet(_sheet) { sheet->generateTileset(firstID); }

		bool operator <(const SheetData& other) const
		{
			return firstID < other.firstID;
		}
	};

	class Tilemap
	{
	private:
		Tilemap() = default;

	public:
		Tilemap(const std::string& name);

		void loadMap(const std::string& name);

		const Ref<Texture2D>& getTex() const { return mTexture; };

		Ref<Framebuffer> getFB() { return mFramebuffer; } // For debug purposes

		static Ref<Tilemap> Create(const std::string& name);

	private:
		// Re-renders the internal framebuffer object and outputs it to a Texture2D
		void GenTex();

		const Ref<Texture2DSheet>& GetSheet(size_t tileID) const;

	private:
		std::string mMapName;
		uint32_t mWidth, mHeight;
		std::vector<SheetData> mSheets;
		std::vector<MapLayer> mLayers;
		Ref<Framebuffer> mFramebuffer;
		Ref<Texture2D> mTexture = nullptr;

	};
}