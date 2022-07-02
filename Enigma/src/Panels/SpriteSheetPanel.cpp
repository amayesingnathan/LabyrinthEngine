#include "SpriteSheetPanel.h"

#include <Labyrinth.h>

#include "../Assets/AssetManager.h"
#include "../Modals/SubTexModal.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace Labyrinth {

	extern const std::filesystem::path gAssetPath;

	SpriteSheetPanel::SpriteSheetPanel()
	{
		mNoTex = AssetManager::GetOrCreate<Texture2D>("NoTex", "assets/textures/checkerboard.png");

		FramebufferSpec fbSpec;
		fbSpec.width = 1;
		fbSpec.height = 1;
		fbSpec.attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.samples = 1;

		mFramebuffer = Framebuffer::Create(fbSpec);
	}

	void SpriteSheetPanel::onUpdate()
	{
		if (mCurrentSubTex)
		{
			mFramebuffer->bind();

			RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 0.0f });
			RenderCommand::Clear();

			Renderer2D::BeginState();
			Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 1.0f, 1.0f }, mCurrentSubTex);
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
			ImGui::Image((ImTextureID)(intptr_t)mCurrentSheet->getBaseTex()->getRendererID(), { mViewportSize.x - 15.0f, 200.0f }, { 0, 1 }, { 1, 0 });
		else
			ImGui::Image((ImTextureID)(intptr_t)mNoTex->getRendererID(), { mViewportSize.x - 15.0f, 200.0f }, { 0, 1 }, { 1, 0 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const FS_CHAR_TYPE* path = (const FS_CHAR_TYPE*)payload->Data;
				std::filesystem::path texturePath = std::filesystem::path(gAssetPath) / path;

				if (std::regex_match(texturePath.extension().string(), Texture2D::GetSuppTypes()))
				{
					mAddType = SheetAddType::Path;
					mNewSheetVar = texturePath.string();
					ImGui::OpenPopup("TileWidthModal");

					mPayload.sheetName = "";
					mTileWidth = 0; mTileHeight = 0;
				}

			}
			ImGui::EndDragDropTarget();
		}
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_MANAGER_ITEM"))
			{
				const std::string& key = *Cast<std::string>(payload->Data);
				if (const Ref<Texture2DSheet>& sheet = AssetManager::Get<Texture2DSheet>(key))
				{
					mCurrentSheet = sheet;
					mSheetWidth = mCurrentSheet->getWidth();
					mSheetHeight = mCurrentSheet->getHeight();
					mFramebuffer->resize(Cast<uint32_t>(mViewportSize.x - 15.0f), 200);
				}
				else if (const Ref<Texture2D>& asset = AssetManager::Get<Texture2D>(key))
				{
					mAddType = SheetAddType::Texture;
					mNewSheetVar = asset;
					ImGui::OpenPopup("TileWidthModal");

					mPayload.sheetName = "";
					mTileWidth = 0; mTileHeight = 0;
				}
				else { LAB_ERROR("Invalid asset type!"); }

			}
			ImGui::EndDragDropTarget();
		}

		if (ImGui::Button("Add Subtexture") && mCurrentSheet)
		{
			mSubTexSelector = new SubTexModal(mPayload);
			ImGui::OpenPopup("SubTexModal");
		}

		ImGui::SameLine();

		if (ImGui::Button("Remove  Subtexture") && mCurrentSheet)
		{
			mCurrentSheet->deleteSubTex(mPayload.subTexName);
			mPayload.subTexName = noSubTex;
			mCurrentSubTex = nullptr;
		}

		if (ImGui::BeginCombo("Subtextures", mPayload.subTexName.c_str()))
		{
			// Display "None" at the top of the list
			bool clear = mPayload.subTexName == noSubTex;
			if (ImGui::Selectable(noSubTex.c_str(), clear))
			{
				mPayload.subTexName = noSubTex;
				mCurrentSubTex = nullptr;
			}

			if (mCurrentSheet)
			{
				for (const auto& [key, value] : mCurrentSheet->getSubTexList())
				{
					bool isSelected = mPayload.subTexName == key;

					if (ImGui::Selectable(key.c_str(), isSelected))
					{
						mPayload.subTexName = key;
						mCurrentSubTex = mCurrentSheet->getSubTex(key);
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}

		ImGui::NewLine();

		if (mCurrentSubTex)
		{
			ImGui::Image((ImTextureID)(intptr_t)mFramebuffer->getColourAttachmentRendererID(), { mViewportSize.x - 15.0f, 200.0f }, { 0, 1 }, { 1, 0 });
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
			{
				ImGui::SetDragDropPayload("SPRITE_SHEET_ITEM", &mPayload, sizeof(SubTexPayload));
				ImGui::EndDragDropSource();
			}
		}
		else ImGui::Image((ImTextureID)(intptr_t)mNoTex->getRendererID(), { mViewportSize.x - 15.0f, 200.0f }, { 0, 1 }, { 1, 0 });

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
		STR_COPY(buffer, mPayload.sheetName);
		if (ImGui::InputText("Name", buffer, sizeof(buffer)))
			mPayload.sheetName = std::string(buffer);

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
			mAddType = SheetAddType::Path;
			mNewSheetVar = std::string();
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();

		if (loadSheet)
		{
			switch (mAddType)
			{
			case SheetAddType::Path:	mCurrentSheet = AssetManager::GetOrCreate<Texture2DSheet>(mPayload.sheetName, std::get<std::string>(mNewSheetVar), glm::vec2{mTileWidth, mTileHeight}, mPayload.sheetName); break;
			case SheetAddType::Texture: mCurrentSheet = AssetManager::GetOrCreate<Texture2DSheet>(mPayload.sheetName, std::get<Ref<Texture2D>>(mNewSheetVar), glm::vec2{ mTileWidth, mTileHeight }, mPayload.sheetName); break;
			}
			
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
		if (!ImGui::BeginPopupModal("SubTexModal", nullptr, flags) || !mSubTexSelector) return;

		mSubTexSelector->display();
		if (mSubTexSelector->complete())
		{
			mCurrentSubTex = mCurrentSheet->getSubTex(mPayload.subTexName);
			delete mSubTexSelector;
		}

		ImGui::EndPopup();
	}
}