#include "Lpch.h"
#include "SceneManager.h"

#include "SceneSerialiser.h"

namespace Laby {

    void SceneManager::Reset()
    {
        mScenes.clear();
        mSceneLoaders.clear();
        mCurrentScene = 0;
        mLoadingScene = 0;
    }

    UUID SceneManager::Preload(const fs::path& scenePath)
    {
        UUID newScene;
        mSceneLoaders.emplace(newScene, scenePath);
        mLoadingScene = newScene;

        return newScene;
    }

    void SceneManager::Load()
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

        mScenes[mLoadingScene] = mSceneLoaders[mLoadingScene].Get();
        mSceneLoaders.erase(mLoadingScene);
        SetCurrentScene(mLoadingScene);
        mLoadingScene = 0;
    }

    void SceneManager::Load(UUID sceneID)
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
            mScenes[sceneID] = mSceneLoaders[sceneID].Get();
            mSceneLoaders.erase(sceneID);
        }

        SetCurrentScene(sceneID);
    }

    UUID SceneManager::Load(const fs::path& scenePath)
    {
        UUID newSceneID;
        mScenes[newSceneID] = SceneLoader(scenePath).Get();
        SetCurrentScene(newSceneID);
        return newSceneID;
    }

    UUID SceneManager::LoadClone(UUID cloneID)
    {
        UUID newSceneID;
        Ref<Scene> toClone = mScenes[cloneID];
        mScenes[newSceneID] = SceneUtils::Clone(toClone);
        SetCurrentScene(newSceneID);
        return newSceneID;
    }

    void SceneManager::Unload(UUID id)
    {
        mScenes.erase(id);
    }

    Ref<Scene> SceneManager::Get(UUID id) const
    {
        if (!mScenes.contains(id))
        {
            LAB_CORE_ERROR("Scene {} does not exist in scene manager!", id);
            return nullptr;
        }

        return mScenes.at(id);
    }

    Ref<Scene> SceneManager::GetActive() const
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

    UUID SceneManager::NewScene(bool load)
    {
        UUID newScene = AssetManager::CreateMemoryOnlyAsset<Scene>();
        mScenes[newScene] = AssetManager::GetAsset<Scene>(newScene);
        return newScene;
    }

    void SceneManager::Save(const fs::path& filepath, UUID sceneToSave)
    {
        if (!sceneToSave)
            sceneToSave = mCurrentScene;

        if (!mScenes.contains(sceneToSave))
        {
            LAB_CORE_ERROR("Scene {} does not exist in scene manager!", sceneToSave);
            return;
        }

        Ref<Scene> scene = mScenes.at(sceneToSave);
        if (AssetManager::IsMemoryAsset(sceneToSave))
        {
            AssetManager::SaveMemoryOnlyAsset<Scene>(scene->GetName(), sceneToSave);
            return;
        }
;
        SceneSerialiser serialiser(scene);
        serialiser.Serialise(filepath);
        AssetManager::ReloadData(sceneToSave);
    }

    void SceneManager::AddSceneChangeCallback(Action<Ref<Scene>>&& action)
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