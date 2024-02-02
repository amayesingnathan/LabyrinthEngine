#pragma once

#include "SceneLoader.h"

namespace Laby {

	class SceneManager : public RefCounted
	{
	public:
		SceneManager() = default;
		SceneManager(const SceneManager&) = delete;
		SceneManager& operator=(const SceneManager&) = delete;

		void Reset();

		UUID Preload(const fs::path& scenePath);
		void Unload(UUID id);

		void Load();
		void Load(UUID sceneID);
		UUID Load(const fs::path& scenePath);
		UUID LoadClone(UUID cloneID);

		Ref<Scene> Get(UUID id) const;
		Ref<Scene> GetActive() const;

		UUID NewScene(bool load = false);
		void Save(const fs::path& filepath, UUID scene = 0);

		void AddSceneChangeCallback(Action<Ref<Scene>>&& action);

	private:
		void SetCurrentScene(UUID scene);

	private:
		UUID mCurrentScene = 0, mLoadingScene = 0;
		std::unordered_map<UUID, Ref<Scene>> mScenes;
		std::unordered_map<UUID, SceneLoader> mSceneLoaders;
		std::vector<Action<Ref<Scene>>> mOnChangeCallbacks;
	};
}