#include "SpriteSheetPanel.h"

#include "../Assets/AssetManager.h"
#include "../Modals/ModalManager.h"
#include "../Modals/SubTexModal.h"
#include "../Modals/TileWidthModal.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace Labyrinth {

	extern const std::filesystem::path gAssetPath;

	SpriteSheetPanel::SpriteSheetPanel()
	{
		//mNoTex = AssetManager::GetOrCreate<Texture2D>("NoTex", "assets/textures/checkerboard.png");

		FramebufferSpec fbSpec;
		fbSpec.width = 1;
		fbSpec.height = 1;
		fbSpec.attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.samples = 1;

		mPanelData.framebuffer = Framebuffer::Create(fbSpec);
	}

	void SpriteSheetPanel::onUpdate()
	{
		if (mPanelData.currentSubTex)
		{
			mPanelData.framebuffer->bind();

			RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 0.0f });
			RenderCommand::Clear();

			Renderer2D::BeginState();
			Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 1.0f, 1.0f }, mPanelData.currentSubTex);
			Renderer2D::EndState();

			mPanelData.framebuffer->unbind();
		}
	}

	void SpriteSheetPanel::onImGuiRender()
	{
		ImGui::Begin("Sprite Sheets");

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		mPanelData.viewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		if (mPanelData.currentSheet)
			ImGui::Image((ImTextureID)(intptr_t)mPanelData.currentSheet->getBaseTex()->getRendererID(), { mPanelData.viewportSize.x - 15.0f, 200.0f }, { 0, 1 }, { 1, 0 });
		else
			ImGui::Image((ImTextureID)(intptr_t)mNoTex->getRendererID(), { mPanelData.viewportSize.x - 15.0f, 200.0f }, { 0, 1 }, { 1, 0 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const FS_CHAR_TYPE* path = (const FS_CHAR_TYPE*)payload->Data;
				std::filesystem::path texturePath = std::filesystem::path(gAssetPath) / path;

				if (std::regex_match(texturePath.extension().string(), Texture2D::GetSuppTypes()))
				{
					mPanelData.addType = SheetAddType::Path;
					mPanelData.newSheetVar = texturePath.string();
					mPanelData.sheetName = "";
					mTileWidth = 0; mTileHeight = 0;

					ModalManager::Open<TileWidthModal>("TileWidthModal", mPanelData);
				}

			}
			ImGui::EndDragDropTarget();
		}
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_MANAGER_ITEM"))
			{
				const AssetHandle& handle = *Cast<AssetHandle>(payload->Data);
				if (const Ref<Texture2DSheet>& sheet = AssetManager::GetAsset<Texture2DSheet>(handle))
				{
					mPanelData.currentSheet = sheet;
					mSheetWidth = mPanelData.currentSheet->getWidth();
					mSheetHeight = mPanelData.currentSheet->getHeight();
					mPanelData.framebuffer->resize(Cast<u32>(mPanelData.viewportSize.x - 15.0f), 200);
				}
				else if (const Ref<Texture2D>& asset = AssetManager::GetAsset<Texture2D>(handle))
				{
					mPanelData.addType = SheetAddType::Texture;
					mPanelData.newSheetVar = asset;
					mPanelData.sheetName = "";
					mTileWidth = 0; mTileHeight = 0;

					ModalManager::Open<SubTexModal>("SubTexModal", mPanelData);
				}
				else { LAB_ERROR("Invalid asset type!"); }

			}
			ImGui::EndDragDropTarget();
		}

		if (ImGui::Button("Add SubTex") && mPanelData.currentSheet)
			ModalManager::Open<SubTexModal>("SubTexModal", mPanelData);

		ImGui::SameLine();

		if (ImGui::Button("Remove SubTex") && mPanelData.currentSheet)
		{
			mPanelData.currentSheet->deleteSubTex(mPanelData.subTexName);
			mPanelData.subTexName = noSubTex;
			mPanelData.currentSubTex = nullptr;
		}

		ImGui::SameLine();

		if (ImGui::Button("Create Tileset") && mPanelData.currentSheet)
			mPanelData.currentSheet->generateTileset();

		if (ImGui::BeginCombo("Subtextures", mPanelData.subTexName.c_str()))
		{
			// Display "None" at the top of the list
			bool clear = mPanelData.subTexName == noSubTex;
			if (ImGui::Selectable(noSubTex.c_str(), clear))
			{
				mPanelData.subTexName = noSubTex;
				mPanelData.currentSubTex = nullptr;
			}

			if (mPanelData.currentSheet)
			{
				for (const auto& [key, value] : mPanelData.currentSheet->getSubTexList())
				{
					bool isSelected = mPanelData.subTexName == key;

					if (ImGui::Selectable(key.c_str(), isSelected))
					{
						mPanelData.subTexName = key;
						mPanelData.currentSubTex = mPanelData.currentSheet->getSubTex(key);
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}

		ImGui::NewLine();

		if (mPanelData.currentSubTex)
		{
			ImGui::Image((ImTextureID)(intptr_t)mPanelData.framebuffer->getColourAttachmentRendererID(), { mPanelData.viewportSize.x - 15.0f, 200.0f }, { 0, 1 }, { 1, 0 });
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
			{
				ImGui::SetDragDropPayload("SPRITE_SHEET_ITEM", &mPanelData, sizeof(SpriteSheetData));
				ImGui::EndDragDropSource();
			}
		}
		else ImGui::Image((ImTextureID)(intptr_t)mNoTex->getRendererID(), { mPanelData.viewportSize.x - 15.0f, 200.0f }, { 0, 1 }, { 1, 0 });

		ImGui::End();
	}
}