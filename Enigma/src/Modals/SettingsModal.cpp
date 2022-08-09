#include "SettingsModal.h"

#include <Labyrinth/Core/Buffer.h>

namespace Labyrinth {

    struct ResolutionEntry
    { 
        std::string label;
        Resolution resolution;
    };

    static const std::vector<ResolutionEntry> sResolutionTable
    {
        {"1280x720", {1280, 720}},
        {"1600x900", {1600, 900}},
        {"1920x1080", {1920, 1080}}
    };

    SettingsModal::SettingsModal() : EditorModal(), mSettings(Application::Get().getSpec())
    {
    }

    void SettingsModal::onImGuiRender()
    {
        ImGui::Text("Settings");

        ImGui::Checkbox("Fullscreen", &mSettings.fullscreen);

        StaticBuffer<256> workingDirBuffer(mSettings.workingDir.string());
        if (ImGui::InputText("Working Directory", workingDirBuffer, sizeof(workingDirBuffer)))
            mSettings.workingDir = workingDirBuffer.string();

        StaticBuffer<256> coreAssemblyBuffer(mSettings.scriptConfig.coreAssemblyPath.string());
        if (ImGui::InputText("Core Assembly Path", coreAssemblyBuffer, sizeof(coreAssemblyBuffer)))
            mSettings.scriptConfig.coreAssemblyPath = coreAssemblyBuffer.string();

        std::string resolutionStr = mSettings.resolution.toString();
        if (ImGui::BeginCombo("Resolution", resolutionStr.c_str()))
        {
            for (const auto& resolutionEntry : sResolutionTable)
            {
                bool isSelected = resolutionStr == resolutionEntry.label;

                if (ImGui::Selectable(resolutionEntry.label.c_str(), isSelected))
                    mSettings.resolution = resolutionEntry.resolution;

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        StaticBuffer<256> projectBuffer(mSettings.startupProject.string());
        if (ImGui::InputText("Startup Project", projectBuffer, projectBuffer.size()))
            mSettings.startupProject = projectBuffer.string();

        ImGui::SameLine();

        if (ImGui::Button("..."))
        {
            fs::path result = FileUtils::OpenFile({ "Labyrinth Project (*.lpj)", "*.lpj" });
            if (!result.empty())
                mSettings.startupProject = fs::relative(result);
        }
    }

    void SettingsModal::onComplete()
    {
        Application::Get().getSpec() = mSettings;
    }
}
