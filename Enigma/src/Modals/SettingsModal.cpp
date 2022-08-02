#include "SettingsModal.h"

#include <Labyrinth/Core/Buffer.h>

namespace Labyrinth {

    SettingsModal::SettingsModal() : Modal(), mSettings(Application::Get().getSpec())
    {
        mSettingsJSON = JSON::Open("enigma.ini");
        if (!mSettingsJSON.contains("Startup"))
        {
            mSettingsJSON["Startup"]["Fullscreen"] = false;
        }

        JsonObj startupSettings = mSettingsJSON["Startup"];

        if (startupSettings.contains("Fullscreen"))
            mSettings.fullscreen = startupSettings["Fullscreen"].get<bool>();
        if (startupSettings.contains("WorkingDir"))
            mSettings.workingDir = startupSettings["WorkingDir"].get<std::filesystem::path>();
        if (startupSettings.contains("ScriptModulePath"))
            mSettings.scriptModulePath = startupSettings["ScriptModulePath"].get<std::filesystem::path>();
    }

    void SettingsModal::onImGuiRender()
    {
        ImGui::Text("Settings");
        ImGui::NewLine();

        ImGui::Checkbox("Fullscreen", &mSettings.fullscreen);

        StaticBuffer<256> workingDirBuffer(mSettings.workingDir.string());
        usize test = sizeof(workingDirBuffer);
        if (ImGui::InputText("Working Directory", workingDirBuffer, sizeof(workingDirBuffer)))
            mSettings.workingDir = workingDirBuffer.string();

        StaticBuffer<256> scriptModuleBuffer(mSettings.scriptModulePath.string());
        if (ImGui::InputText("Script Module Path", scriptModuleBuffer, sizeof(scriptModuleBuffer)))
            mSettings.workingDir = scriptModuleBuffer.string();

        ImGui::NewLine();

        if (ImGui::Button("OK"))
        {
            Save();
            Close();
        }

        ImGui::SameLine();
        if (ImGui::Button("Apply"))
            Save();

        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
            Close();
    }

    void SettingsModal::Save()
    {
        JsonObj& startupSettings = mSettingsJSON["Startup"];

        startupSettings["Fullscreen"] = mSettings.fullscreen;
        startupSettings["WorkingDir"] = mSettings.workingDir;
        startupSettings["ScriptModulePath"] = mSettings.scriptModulePath;

        JSON::Write("enigma.ini", mSettingsJSON);
    }
}
