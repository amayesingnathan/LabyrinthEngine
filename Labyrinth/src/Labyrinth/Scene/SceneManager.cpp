#include "Lpch.h"
#include "SceneManager.h"

namespace Laby {

    void SceneManager::reset()
    {
        mScenes.clear();
        mSceneLoaders.clear();
        mCurrentScene = 0;
        mLoadingScene = 0;
    }

    UUID SceneManager::preload(const fs::path& scenePath)
    {
        UUID newScene;
        mSceneLoaders.emplace(newScene, scenePath);
        mLoadingScene = newScene;

        return newScene;
    }

    void SceneManager::load()
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

    void SceneManager::load(UUID sceneID)
    {
        bool preloaded = mSceneLoaders.contains(sceneID);
        bool loaded = mScenes.contains(sceneID);

        if (!preloaded && !loaded)
        {
            LAB_CORE_ERROR("This scene does not exist in the scene manager!");
            return;
        }

        if (preloaded)
        {
            mScenes[sceneID] = mSceneLoaders[sceneID].get();
            mSceneLoaders.erase(sceneID);
        }

        SetCurrentScene(sceneID);
    }

    UUID SceneManager::load(const fs::path& scenePath)
    {
        UUID newSceneID;
        mScenes[newSceneID] = SceneLoader(scenePath).get();
        SetCurrentScene(newSceneID);
        return newSceneID;
    }

    UUID SceneManager::loadClone(UUID cloneID)
    {
        UUID newSceneID;
        Ref<Scene> toClone = mScenes[cloneID];
        mScenes[newSceneID] = SceneUtils::Clone(toClone);
        SetCurrentScene(newSceneID);
        return newSceneID;
    }

    void SceneManager::unload(UUID id)
    {
        mScenes.erase(id);
    }

    Ref<Scene> SceneManager::get(UUID id) const
    {
        if (!mScenes.contains(id))
        {
            LAB_CORE_ERROR("Scene {} does not exist in scene manager!", id);
            return nullptr;
        }

        return mScenes.at(id);
    }

    Ref<Scene> SceneManager::getActive() const
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

    UUID SceneManager::newScene()
    {
        UUID newScene = AssetManager::CreateMemoryOnlyAsset<Scene>();
        mScenes[newScene] = AssetManager::GetAsset<Scene>(newScene);
        return newScene;
    }

    void SceneManager::addSceneChangeCallback(Action<Ref<Scene>>&& action)
    {
        mOnChangeCallbacks.emplace_back(std::move(action));
    }

    void SceneManager::SetCurrentScene(UUID scene)
    {
        for (const auto& action : mOnChangeCallbacks)
            action(mScenes[scene]);

        mCurrentScene = scene;
    }
}