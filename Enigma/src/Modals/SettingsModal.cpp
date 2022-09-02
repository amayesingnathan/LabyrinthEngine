#include "SettingsModal.h"

#include <Labyrinth/Containers/StaticString.h>

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

        StaticString<256> workingDirBuffer(mSettings.workingDir.string());
        if (ImGui::InputText("Working Directory", workingDirBuffer, workingDirBuffer.length()))
            mSettings.workingDir = workingDirBuffer.toString();

        StaticString<256> coreAssemblyBuffer(mSettings.scriptConfig.coreAssemblyPath.string());
        if (ImGui::InputText("Core Assembly Path", coreAssemblyBuffer, workingDirBuffer.length()))
            mSettings.scriptConfig.coreAssemblyPath = coreAssemblyBuffer.toString();

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

        StaticString<256> projectBuffer(mSettings.startupProject.string());
        if (ImGui::InputText("Startup Project", projectBuffer, projectBuffer.length()))
            mSettings.startupProject = projectBuffer.toString();

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
