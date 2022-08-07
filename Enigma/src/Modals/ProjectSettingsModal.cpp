#include "ProjectSettingsModal.h"

#include <Labyrinth/Assets/AssetManager.h>
#include <Labyrinth/Core/Buffer.h>
#include <Labyrinth/IO/Filesystem.h>

namespace Labyrinth {

    ProjectSettingsModal::ProjectSettingsModal(const Ref<Project>& project) : Modal(), mProject(project)
    {
    }

    void ProjectSettingsModal::onImGuiRender()
    {
        ImGui::Text("Project Settings");

        {   // Project Name
            StaticBuffer<256> buffer(mProject->mSettings.name);
            if (ImGui::InputText("Name", buffer, buffer.size()))
                mProject->mSettings.name = buffer.string();
        }
        {   // Asset Directory
            if (ImGui::Button("Asset Directory"))
            {
                fs::path result = FileUtils::OpenDir();
                if (!result.empty())
                    mProject->mSettings.assetDir = result;
            }
            std::string label = !mProject->mSettings.assetDir.empty() ? mProject->mSettings.assetDir.string() : "...";
            ImGui::Text(label.c_str());
        }
        {   // Asset Registry Path
            if (ImGui::Button("Asset Registry Path"))
            {
                fs::path result = FileUtils::OpenFile({ "Asset Registry (*.lreg)", "*.lreg" });
                if (!result.empty())
                    mProject->mSettings.assetRegPath = result;
            }
            std::string label = !mProject->mSettings.assetRegPath.empty() ? mProject->mSettings.assetRegPath.string() : "...";
            ImGui::Text(label.c_str());
        }

        ImGui::Separator();

        {   // Script Module Path
            if (ImGui::Button("Script Assembly Directory"))
            {
                fs::path result = FileUtils::OpenDir();
                if (!result.empty())
                    mProject->mSettings.scriptModulePath = result;
            }
            std::string label = !mProject->mSettings.scriptModulePath.empty() ? mProject->mSettings.scriptModulePath.string() : "...";
            ImGui::Text(label.c_str());
        }

        ImGui::Separator();

        {   // Project Directory
            if (ImGui::Button("Project Directory"))
            {
                fs::path result = FileUtils::OpenDir();
                if (!result.empty())
                    mProject->mSettings.projectDir = result;
            }
            std::string label = !mProject->mSettings.projectDir.empty() ? mProject->mSettings.projectDir.string() : "...";
            ImGui::Text(label.c_str());
        }
        {   // Default Scene
            if (ImGui::Button("Default Scene"))
            {
                fs::path result = FileUtils::OpenFile({ "Labyrinth Scene (.lscene)", "*.lscene" });
                if (!result.empty())
                    mProject->mSettings.startScenePath = result;
            }
            std::string label = !mProject->mSettings.startScenePath.empty() ? mProject->mSettings.startScenePath.string() : "...";
            ImGui::Text(label.c_str());
        }

        ImGui::Separator();

        if (ImGui::Button("Close"))
            Close();
    }
}
