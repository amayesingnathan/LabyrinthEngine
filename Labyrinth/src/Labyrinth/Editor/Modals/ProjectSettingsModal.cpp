#include "Lpch.h"
#include "ProjectSettingsModal.h"

#include <Labyrinth/Assets/AssetManager.h>
#include <Labyrinth/ImGui/ImGuiCpp.h>
#include <Labyrinth/IO/Filesystem.h>

using imcpp::Widgets;

namespace Laby{

    ProjectSettingsModal::ProjectSettingsModal(const Ref<Project>& project) 
        : mProject(project)
    {
    }

    void ProjectSettingsModal::onImGuiRender()
    {
        Widgets::Label("Project Settings");

        Widgets::StringEdit("Name", mProject->mSettings.name);

        Widgets::Button("Asset Directory", [&, this]()
        {
            fs::path result = FileUtils::OpenDir();
            if (!result.empty())
                mProject->mSettings.assetDir = fs::relative(result);
        });
        Widgets::Label(mProject->mSettings.assetDir.string());

        Widgets::Button("Asset Registry Path", [&, this]()
        {
            fs::path result = FileUtils::OpenFile({ "Asset Registry (*.lreg)", "*.lreg" });
            if (!result.empty())
                mProject->mSettings.assetRegPath = fs::relative(result);
        });
        Widgets::Label(mProject->mSettings.assetRegPath.string());

        Widgets::Separator();

        Widgets::Button("Script Assembly Directory", [&, this]()
        {
            fs::path result = FileUtils::OpenDir();
            if (!result.empty())
                mProject->mSettings.scriptModulePath = fs::relative(result, mProject->mSettings.projectDir);
        });
        Widgets::Label(mProject->mSettings.scriptModulePath.string());

        Widgets::Separator();

        Widgets::Button("Project Directory", [&, this]()
        {
            fs::path result = FileUtils::OpenDir();
            if (!result.empty())
                mProject->mSettings.projectDir = fs::relative(result);
        });
        Widgets::Label(mProject->mSettings.projectDir.string());

        Widgets::Button("Default Scene", [&, this]()
        {
            fs::path result = FileUtils::OpenFile({ "Labyrinth Scene (.lscene)", "*.lscene" });
            if (!result.empty())
                mProject->mSettings.startScenePath = fs::relative(result, mProject->mSettings.projectDir);
        });
        Widgets::Label(mProject->mSettings.startScenePath.string());
    }
}
