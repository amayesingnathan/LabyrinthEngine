#include "SettingsModal.h"

namespace Labyrinth {

    SettingsModal::SettingsModal() : Modal()
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
        else
            mSettings.workingDir = std::filesystem::current_path();
    }

    void SettingsModal::onImGuiRender()
    {
        ImGui::Text("Settings");
        ImGui::NewLine();

        ImGui::Checkbox("Fullscreen", &mSettings.fullscreen);

        char buffer[256];
        memset(buffer, 0, sizeof(buffer));
        STR_COPY(buffer, mSettings.workingDir.string());
        if (ImGui::InputText("Working Directory", buffer, sizeof(buffer)))
            mSettings.workingDir = buffer;

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

        JSON::Write("enigma.ini", mSettingsJSON);
    }
}
