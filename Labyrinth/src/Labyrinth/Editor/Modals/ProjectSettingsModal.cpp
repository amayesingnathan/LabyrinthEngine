#include "Lpch.h"
#include "ProjectSettingsModal.h"

#include <Labyrinth/Assets/AssetManager.h>
#include <Labyrinth/Containers/StaticString.h>
#include <Labyrinth/IO/Filesystem.h>

#include <imgui.h>

namespace Laby{

    ProjectSettingsModal::ProjectSettingsModal(const Ref<Project>& project) 
        : mProject(project)
    {
    }

    void ProjectSettingsModal::onImGuiRender()
    {
        ImGui::Text("Project Settings");

        {   // Project Name
            StaticString<256> buffer(mProject->mSettings.name);
            if (ImGui::InputText("Name", buffer, buffer.length()))
                mProject->mSettings.name = buffer.toString();
        }
        {   // Asset Directory
            if (ImGui::Button("Asset Directory"))
            {
                fs::path result = FileUtils::OpenDir();
                if (!result.empty())
                    mProject->mSettings.assetDir = fs::relative(result);
            }
            std::string label = !mProject->mSettings.assetDir.empty() ? mProject->mSettings.assetDir.string() : "...";
            ImGui::Text(label.c_str());
        }
        {   // Asset Registry Path
            if (ImGui::Button("Asset Registry Path"))
            {
                fs::path result = FileUtils::OpenFile({ "Asset Registry (*.lreg)", "*.lreg" });
                if (!result.empty())
                    mProject->mSettings.assetRegPath = fs::relative(result);
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
                    mProject->mSettings.scriptModulePath = fs::relative(result, mProject->mSettings.projectDir);
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
                    mProject->mSettings.projectDir = fs::relative(result);
            }
            std::string label = !mProject->mSettings.projectDir.empty() ? mProject->mSettings.projectDir.string() : "...";
            ImGui::Text(label.c_str());
        }
        {   // Default Scene
            if (ImGui::Button("Default Scene"))
            {
                fs::path result = FileUtils::OpenFile({ "Labyrinth Scene (.lscene)", "*.lscene" });
                if (!result.empty())
                    mProject->mSettings.startScenePath = fs::relative(result, mProject->mSettings.projectDir);
            }
            std::string label = !mProject->mSettings.startScenePath.empty() ? mProject->mSettings.startScenePath.string() : "...";
            ImGui::Text(label.c_str());
        }
    }
}
