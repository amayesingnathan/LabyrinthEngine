#pragma once

#include <future>

#include <Labyrinth/Assets/AssetManager.h>
#include <Labyrinth/IO/Filesystem.h>

#include "Scene.h"

namespace Laby {

	class SceneLoader
	{
	public:
		using SceneLoaderResult = std::future<Ref<Scene>>;

	public:
		SceneLoader() = default;
		SceneLoader(const fs::path& scenePath)
			: mResult(std::async(std::launch::async, [scenePath] { return AssetManager::GetAsset<Scene>(scenePath); }))
		{
		}

		Ref<Scene> get()
		{
			return mResult.get();
		}

	private:
		SceneLoaderResult mResult;
	};
}