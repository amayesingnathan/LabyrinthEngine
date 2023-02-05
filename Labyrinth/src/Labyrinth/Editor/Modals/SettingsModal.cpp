#include "Lpch.h"
#include "SettingsModal.h"

#include <Labyrinth/ImGui/ImGuiCpp.h>

using imcpp::Widgets;

namespace Laby {

    using ResolutionEntry = imcpp::ComboEntry<Resolution>;

    static constexpr std::array<ResolutionEntry, 3> sResolutionTable
    {
        ResolutionEntry{ "1280x720", Resolution{1280, 720} },
        ResolutionEntry{ "1600x900", Resolution{1600, 900} },
        ResolutionEntry{ "1920x1080", Resolution{1920, 1080} }
    };

    SettingsModal::SettingsModal() 
        : mSettings(Application::GetSpec())
    {
    }

    void SettingsModal::onImGuiRender()
    {
        Widgets::Label("Settings");

        Widgets::Checkbox("Fullscreen", mSettings.fullscreen);
        Widgets::PathEdit("Working Directory", mSettings.workingDir);
        Widgets::PathEdit("Core Assembly Path", mSettings.scriptConfig.coreAssemblyPath);
        Widgets::Combobox<Resolution>("Resolution", mSettings.resolution.toString(), mSettings.resolution, sResolutionTable);

        Widgets::PathEdit("Startup Project", mSettings.startupProject);
        Widgets::SameLine();
        Widgets::Button("...", [&, this]
        {
            fs::path result = FileUtils::OpenFile({ "Labyrinth Project (*.lpj)", "*.lpj" });
            if (!result.empty())
                mSettings.startupProject = fs::relative(result);
        });
    }

    void SettingsModal::onComplete()
    {
        Application::Get().mSpecification = mSettings;
        Application::WriteSettings("enigma.json");
    }
}
