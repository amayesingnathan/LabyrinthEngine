#pragma once

#include "MapLayer.h"

#include <Labyrinth/Assets/AssetGroup.h>
#include <Labyrinth/Core/System/Base.h>
#include <Labyrinth/Renderer/Framebuffer.h>

namespace Labyrinth {

	struct SheetData
	{
		usize firstID;
		Ref<Texture2DSheet> sheet;

		SheetData(usize id, const Ref<Texture2DSheet>& tex) : firstID(id), sheet(tex) {}

		bool operator <(const SheetData& other) const
		{
			return firstID < other.firstID;
		}
	};

	class Tilemap : public RefCounted
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

		const Ref<Texture2DSheet>& GetSheet(usize tileID) const;

	private:
		std::string mMapName;
		usize mWidth, mHeight;
		std::vector<SheetData> mSheets;
		std::vector<MapLayer> mLayers;
		Ref<Framebuffer> mFramebuffer;
		Ref<Texture2D> mTexture = nullptr;

	};
}