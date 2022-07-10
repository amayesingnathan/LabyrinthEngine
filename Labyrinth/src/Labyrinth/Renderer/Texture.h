#pragma once

#include "Labyrinth/Core/System/Base.h"
#include "Labyrinth/Assets/IAsset.h"

#include <string>
#include <regex>

namespace Labyrinth {

	class LAB_API Texture : public IAsset
	{
	public:
		virtual ~Texture() = default;

		virtual const std::string& getPath() const = 0;
		virtual size_t getWidth() const = 0;
		virtual size_t getHeight() const = 0;
		virtual uint32_t getRendererID() const = 0;

		virtual void setData(void* data, size_t size) = 0;

		virtual void bind(uint32_t slot = 0) const = 0;

		virtual bool operator==(const Texture& other) const = 0;
	};

	class LAB_API Texture2D : public Texture
	{
	public:
		virtual ~Texture2D() = default;

		static Ref<Texture2D> Create(size_t width, size_t height);
		static Ref<Texture2D> Create(const std::string& path);

		static const std::regex GetSuppTypes() { return mSuppImgTypes; }

	private:
		static const std::regex mSuppImgTypes;
	};

}