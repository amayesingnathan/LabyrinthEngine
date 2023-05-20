#pragma once

#include "SceneLoader.h"

namespace Laby {

	class SceneManager : public RefCounted
	{
	public:
		SceneManager() = default;
		SceneManager(const SceneManager&) = delete;
		SceneManager& operator=(const SceneManager&) = delete;

		void reset()
		{
			mScenes.clear();
			mSceneLoaders.clear();
			mCurrentScene = 0;
			mLoadingScene = 0;
		}

		UUID preload(const fs::path& scenePath)
		{
			UUID newScene;
			mSceneLoaders.emplace(newScene, scenePath);
			mLoadingScene = newScene;

			return newScene;
		}

		void load()
		{
			if (!mLoadingScene)
			{
				LAB_CORE_WARN("No scene was preloaded to load now!");
				return;
			}
			if (!mSceneLoaders.contains(mLoadingScene))
			{
				LAB_CORE_ERROR("This scene loader could not be found for the in progress scene!");
				return;
			}

			mScenes[mLoadingScene] = mSceneLoaders[mLoadingScene].get();
			mSceneLoaders.erase(mLoadingScene);
			SetCurrentScene(mLoadingScene);
			mLoadingScene = 0;
		}

		Ref<Scene> load(UUID sceneID)
		{
			bool preloaded = mSceneLoaders.contains(sceneID);
			bool loaded = mScenes.contains(sceneID);

			if (!preloaded && !loaded)
			{
				LAB_CORE_ERROR("This scene does not exist in the scene manager!");
				return nullptr;
			}

			if (preloaded)
			{
				mScenes[sceneID] = mSceneLoaders[sceneID].get();
				mSceneLoaders.erase(sceneID);
			}

			SetCurrentScene(sceneID);
			return mScenes[mCurrentScene];
		}

		UUID load(const fs::path& scenePath)
		{
			UUID newSceneID;
			mScenes[newSceneID] = SceneLoader(scenePath).get();
			SetCurrentScene(newSceneID);
			return newSceneID;
		}

		UUID loadClone(UUID cloneID)
		{
			UUID newSceneID;
			Ref<Scene> toClone = mScenes[cloneID];
			mScenes[newSceneID] = SceneUtils::Clone(toClone);
			SetCurrentScene(newSceneID);
			return newSceneID;
		}

		void unload(UUID id)
		{
			mScenes.erase(id);
		}

		Ref<Scene> get(UUID id) const
		{
			if (!mScenes.contains(id))
			{
				LAB_CORE_ERROR("Scene {} does not exist in scene manager!", id);
				return nullptr;
			}

			return mScenes.at(id);
		}

		Ref<Scene> getActive() const
		{
			if (!mCurrentScene)
				return nullptr;

			if (!mScenes.contains(mCurrentScene))
			{
				LAB_CORE_ERROR("Currently active scene {} does not exist in scene manager!", mCurrentScene);
				return nullptr;
			}

			return mScenes.at(mCurrentScene);
		}

		UUID newScene()
		{
			UUID newScene;
			mScenes[newScene] = Ref<Scene>::Create();
			return newScene;
		}

		void addSceneChangeCallback(Action<Ref<Scene>>&& action)
		{
			mOnChangeCallbacks.emplace_back(std::move(action));
		}

	private:
		void SetCurrentScene(UUID scene)
		{
			for (const auto& action : mOnChangeCallbacks)
				action(mScenes[scene]);

			mCurrentScene = scene;
		}

	private:
		UUID mCurrentScene = 0, mLoadingScene = 0;
		std::unordered_map<UUID, Ref<Scene>> mScenes;
		std::unordered_map<UUID, SceneLoader> mSceneLoaders;
		std::vector<Action<Ref<Scene>>> mOnChangeCallbacks;
	};
}