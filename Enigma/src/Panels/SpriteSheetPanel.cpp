#include "SpriteSheetPanel.h"

#include <Labyrinth.h>

#include <imgui/imgui.h>

namespace Labyrinth {

	extern const std::filesystem::path gAssetPath;

	SpriteSheetPanel::SpriteSheetPanel()
	{
		mNoSheet = Texture2D::Create("assets/textures/checkerboard.png");

		FramebufferSpec fbSpec;
		fbSpec.width = 1;
		fbSpec.height = 1;
		fbSpec.attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.samples = 1;

		mFramebuffer = Framebuffer::Create(fbSpec);
	}

	void SpriteSheetPanel::onUpdate(Timestep ts)
	{
		if (mPayload.mSelectedSubTex)
		{
			mFramebuffer->bind();

			RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 0.0f });
			RenderCommand::Clear();

			Renderer2D::BeginState();
			Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 1.0f, 1.0f }, mPayload.mSelectedSubTex);
			Renderer2D::EndState();

			mFramebuffer->unbind();
		}
	}

	void SpriteSheetPanel::onImGuiRender()
	{
		ImGui::Begin("Sprite Sheets");

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		mViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		if (mCurrentSheet)
			ImGui::Image((ImTextureID)(intptr_t)mCurrentSheet->getTex()->getRendererID(), { mViewportSize.x - 15.0f, 200.0f }, { 0, 1 }, { 1, 0 });
		else
			ImGui::Image((ImTextureID)(intptr_t)mNoSheet->getRendererID(), { mViewportSize.x - 15.0f, 200.0f }, { 0, 1 }, { 1, 0 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const FS_CHAR_TYPE* path = (const FS_CHAR_TYPE*)payload->Data;
				std::filesystem::path texturePath = std::filesystem::path(gAssetPath) / path;

				if (std::regex_match(texturePath.extension().string(), Texture2D::GetSuppTypes()))
				{
					mCurrentSheetPath = texturePath.string();
					ImGui::OpenPopup("TileWidthModal");

					mSheetName = "";
					mTileWidth = 0; mTileHeight = 0;
				}

			}
			ImGui::EndDragDropTarget();
		}

		if (ImGui::Button("Add Subtexture") && mCurrentSheet)
		{
			ImGui::OpenPopup("SubTexModal");
			Application::BlockEsc();
		}

		ImGui::SameLine();

		if (ImGui::Button("Remove  Subtexture") && mCurrentSheet)
		{
			mCurrentSheet->deleteSubTex(mPayload.mSelectedSubTexName);
			mPayload.mSelectedSubTex = nullptr;
			mPayload.mSelectedSubTexName = noSubTex;
		}

		if (ImGui::BeginCombo("Subtextures", mPayload.mSelectedSubTexName.c_str()))
		{
			// Display "None" at the top of the list
			bool clear = mPayload.mSelectedSubTexName == noSubTex;
			if (ImGui::Selectable(noSubTex.c_str(), clear))
			{
				mPayload.mSelectedSubTexName = noSubTex;
				mPayload.mSelectedSubTex = nullptr;
			}

			if (mCurrentSheet)
			{
				for (const auto& [key, value] : mCurrentSheet->getSubTexList())
				{
					bool isSelected = mPayload.mSelectedSubTexName == key;

					if (ImGui::Selectable(key.c_str(), isSelected))
					{
						mPayload.mSelectedSubTexName = key;
						mPayload.mSelectedSubTex = value;
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}

		ImGui::NewLine();

		if (mPayload.mSelectedSubTex)
		{
			ImGui::Image((ImTextureID)(intptr_t)mFramebuffer->getColourAttachmentRendererID(), { mViewportSize.x - 15.0f, 200.0f }, { 0, 1 }, { 1, 0 });
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
			{
				ImGui::SetDragDropPayload("SPRITE_SHEET_ITEM", &mPayload, sizeof(SubTexPayload));
				ImGui::EndDragDropSource();
			}
		}
		else ImGui::Image((ImTextureID)(intptr_t)mNoSheet->getRendererID(), { mViewportSize.x - 15.0f, 200.0f }, { 0, 1 }, { 1, 0 });

		TileWidthModal();
		SubTexModalRender();

		ImGui::End();
	}

	void SpriteSheetPanel::TileWidthModal()
	{
		if (!ImGui::BeginPopupModal("TileWidthModal")) return;

		ImGui::Text("Please enter the width and height of each tile in the sprite sheet:");
		ImGui::NewLine();

		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		STR_COPY(buffer, mSheetName);
		if (ImGui::InputText("Name", buffer, sizeof(buffer)))
		{
			mSheetName = std::string(buffer);
		}

		ImGui::InputInt("Width", &mTileWidth);
		ImGui::InputInt("Height", &mTileHeight);
		ImGui::NewLine();

		bool loadSheet = false;

		if (ImGui::Button("OK"))
		{
			if (mTileWidth > 0 && mTileHeight > 0)
			{
				loadSheet = true;
				ImGui::CloseCurrentPopup();
			}
		}

		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();

		if (loadSheet)
		{
			mCurrentSheet.reset();
			mCurrentSheet = Texture2DSheet::CreateFromPath(mCurrentSheetPath, { mTileWidth, mTileHeight }, mSheetName);
			mSheetWidth = mCurrentSheet->getWidth();
			mSheetHeight = mCurrentSheet->getHeight();
			mFramebuffer->resize(Cast<uint32_t>(mViewportSize.x - 15.0f), 200);
		}
		
	}

	void SpriteSheetPanel::SubTexModalRender() 
	{
		ImVec2 centre = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(centre, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		ImGuiWindowFlags flags = ImGuiWindowFlags_None;
		if (!ImGui::BeginPopupModal("SubTexModal", nullptr, flags)) return;

		mSubTexSelector.display(mCurrentSheet, mPayload);

		ImGui::EndPopup();
				
	}
}