#pragma once

#include "SceneLoader.h"

namespace Laby {

	class SceneManager : public RefCounted
	{
	public:
		SceneManager() = default;
		SceneManager(const SceneManager&) = delete;
		SceneManager& operator=(const SceneManager&) = delete;

		void reset();

		UUID preload(const fs::path& scenePath);
		void unload(UUID id);

		void load();
		void load(UUID sceneID);
		UUID load(const fs::path& scenePath);
		UUID loadClone(UUID cloneID);

		Ref<Scene> get(UUID id) const;
		Ref<Scene> getActive() const;

		UUID newScene(bool load = false);
		void save(const fs::path& filepath, UUID scene = 0);

		void addSceneChangeCallback(Action<Ref<Scene>>&& action);

	private:
		void SetCurrentScene(UUID scene);

	private:
		UUID mCurrentScene = 0, mLoadingScene = 0;
		std::unordered_map<UUID, Ref<Scene>> mScenes;
		std::unordered_map<UUID, SceneLoader> mSceneLoaders;
		std::vector<Action<Ref<Scene>>> mOnChangeCallbacks;
	};
}