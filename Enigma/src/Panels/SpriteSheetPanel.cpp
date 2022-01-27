#include "SpriteSheetPanel.h"

#include <Labyrinth.h>

#include <imgui/imgui.h>

namespace Labyrinth {

	extern const std::filesystem::path gAssetPath;

	SpriteSheetPanel::SpriteSheetPanel()
	{
		mNoSheet = Texture2D::Create("assets/textures/empty.png");

		FramebufferSpec fbSpec;
		fbSpec.width = 1600;
		fbSpec.height = 900;
		fbSpec.attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.samples = 1;

		mFramebuffer = Framebuffer::Create(fbSpec);
	}

	void SpriteSheetPanel::onImGuiRender()
	{
		ImGui::Begin("Sprite Sheets");

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		mViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		float viewportWidth = ImGui::GetWindowSize().x;
		if (mCurrentSheet)
			ImGui::Image((void*)mCurrentSheet->getTex()->getRendererID(), { viewportWidth - 15.0f, 200.0f }, { 0, 1 }, { 1, 0 });
		else
			ImGui::Image((void*)mNoSheet->getRendererID(), { viewportWidth - 15.0f, 200.0f }, { 0, 1 }, { 1, 0 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				std::filesystem::path texturePath = std::filesystem::path(gAssetPath) / path;

				if (texturePath.extension().string() == ".png")
				{
					mCurrentSheetPath = texturePath.string();
					ImGui::OpenPopup("TileWidthModal");

					mTileWidth = 0; mTileHeight = 0;
				}

			}
			ImGui::EndDragDropTarget();
		}

		if (ImGui::Button("Add Subtexture") && mCurrentSheet)
		{
			ImGui::OpenPopup("SubTexModal");
			mSubTexModalOpen = true;
		}

		std::string currentSubTexString = "None";
		if (ImGui::BeginCombo("Subtextures", currentSubTexString.c_str()))
		{
			if (mCurrentSheet)
			{
				for (const auto& [key, value] : mCurrentSheet->getSubTexList())
				{
					bool isSelected = currentSubTexString == key;

					if (ImGui::Selectable(key.c_str(), isSelected))
					{
						currentSubTexString = key;
						mSelectedSubTex = value;
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}

		TileWidthModal();
		SubTexModalRender();

		ImGui::End();
	}

	void SpriteSheetPanel::TileWidthModal()
	{
		if (!ImGui::BeginPopupModal("TileWidthModal")) return;

		ImGui::Text("Please enter the width and height of each tile in the sprite sheet:");
		ImGui::NewLine();

		ImGui::InputInt("Width", &mTileWidth);
		ImGui::InputInt("Height", &mTileHeight);
		ImGui::NewLine();

		bool loadSheet = false;

		if (ImGui::Button("OK"))
			if (mTileWidth > 0 && mTileHeight > 0)
			{
				loadSheet = true;
				ImGui::CloseCurrentPopup();
				mSubTexModalOpen = false;
			}

		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
			mSubTexModalOpen = false;
		}

		ImGui::EndPopup();

		if (loadSheet)
		{
			mCurrentSheet = Texture2DSheet::CreateFromPath(mCurrentSheetPath, { mTileWidth, mTileHeight });
			mSheetWidth = mCurrentSheet->getWidth();
			mSheetHeight = mCurrentSheet->getHeight();
			mFramebuffer->resize(Cast<uint32_t>(mSheetWidth), Cast<uint32_t>(mSheetHeight));
		}
		
	}

	void SpriteSheetPanel::SubTexModalRender() 
	{
		ImVec2 centre = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(centre, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (!ImGui::BeginPopupModal("SubTexModal", nullptr, ImGuiWindowFlags_NoResize)) return;

		subTexSelector.init(mCurrentSheet);

		subTexSelector.display();


		ImGui::EndPopup();
				
	}
}