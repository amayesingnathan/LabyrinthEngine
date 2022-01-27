#include "SubTexModal.h"

namespace Labyrinth {

	void SubTexModal::init(Ref<Texture2DSheet> sheet)
	{
		uint32_t widthCount = sheet->getTileCountX();
		uint32_t heightCount = sheet->getTileCountY();

		for (int i = 0; i < widthCount; i++)
			for (int j = 0; j < heightCount; j++)
				mSquares.emplace(std::make_pair(i, j), false);

		mSheet = sheet;
	}

	void SubTexModal::display()
	{
		uint32_t widthCount = mSheet->getTileCountX();
		uint32_t heightCount = mSheet->getTileCountY();

		std::string newKey;
		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		if (ImGui::InputText("Name", buffer, sizeof(buffer)))
		{
			newKey = std::string(buffer);
		}

		float ypos = ImGui::GetCursorPosY();
		float xpos = ImGui::GetCursorPosX();

		auto imageSize = ImGui::GetWindowSize() - 100.0f;
		ImVec2 tileSize = { imageSize.x / widthCount, imageSize.y / heightCount };
		ImGui::Image((void*)mSheet->getTex()->getRendererID(), { imageSize.x, imageSize.y }, { 0, 1 }, { 1, 0 });

		const ImVec4 onColour = {};

		auto& colours = ImGui::GetStyle().Colors;
		const auto& buttonHovered = colours[ImGuiCol_ButtonHovered];
		const auto& buttonActive = colours[ImGuiCol_ButtonActive];

		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

		for (auto& [pos, pressed] : mSquares)
		{
			std::string name = "##Button" + std::to_string(pos.first) + std::to_string(pos.second);
			ImGui::SetCursorPosX(xpos + (pos.first * tileSize.y));
			ImGui::SetCursorPosY(ypos + (pos.second * tileSize.y));

			if (pressed)
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));
			else
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

			if (ImGui::Button(name.c_str(), tileSize))
			{
				pressed = !pressed;
			}

			ImGui::PopStyleColor();

		}

		ImGui::PopStyleColor(2);

		bool loadSheet = false;

		if (ImGui::Button("OK"))
		{
			loadSheet = true;
			ImGui::CloseCurrentPopup();
		}

		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}
	}

}