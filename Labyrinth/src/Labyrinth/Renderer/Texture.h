#pragma once

#include "Labyrinth/Core/System/Base.h"
#include "Labyrinth/Assets/Asset.h"

#include <string>
#include <regex>

namespace Labyrinth {

	class Texture : public Asset
	{
	public:
		ASSET_STATIC_TYPE(AssetType::Texture)

	public:
		virtual ~Texture() = default;

		virtual const std::string& getPath() const = 0;
		virtual i32 getWidth() const = 0;
		virtual i32 getHeight() const = 0;
		virtual u32 getRendererID() const = 0;

		virtual void setData(void* data, i32 size) = 0;

		virtual void bind(u32 slot = 0) const = 0;

		virtual bool operator==(const Texture& other) const = 0;

	};

	class Texture2D : public Texture
	{
	public:
		virtual ~Texture2D() = default;

		virtual bool loaded() const { return false; };

		static Ref<Texture2D> Create(i32 width, i32 height);
		static Ref<Texture2D> Create(const std::string& path);
	};

}