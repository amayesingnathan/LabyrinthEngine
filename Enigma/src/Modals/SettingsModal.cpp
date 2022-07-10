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
    }

    void SettingsModal::onImGuiRender()
    {
        ImGui::Text("Settings");
        ImGui::NewLine();

        ImGui::Checkbox("Fullscreen", &mSettings.fullscreen);

        ImGui::NewLine();

        if (ImGui::Button("OK"))
        {
            Save();
            Close();
        }

        ImGui::SetItemDefaultFocus();
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

        JSON::Write("enigma.ini", mSettingsJSON);
    }
}
