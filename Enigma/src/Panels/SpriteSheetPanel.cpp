#include "SpriteSheetPanel.h"

#include "../Modals/SubTexModal.h"

#include <Labyrinth/Assets/AssetManager.h>
#include <Labyrinth/Containers/StaticString.h>
#include <Labyrinth/Editor/ModalManager.h>
#include <Labyrinth/Editor/EditorResources.h>
#include <Labyrinth/Renderer/Renderer2D.h>
#include <Labyrinth/Renderer/RenderCommand.h>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace Labyrinth {

	SpriteSheetPanel::SpriteSheetPanel()
	{
		FramebufferSpec fbSpec;
		fbSpec.width = 1;
		fbSpec.height = 1;
		fbSpec.attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.samples = 1;

		mPanelData.framebuffer = Framebuffer::Create(fbSpec);
	}

	void SpriteSheetPanel::onUpdate(Timestep ts)
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
			ImGui::Image((ImTextureID)(intptr_t)EditorResources::NoTexture->getRendererID(), { mPanelData.viewportSize.x - 15.0f, 200.0f }, { 0, 1 }, { 1, 0 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const FS_CHAR_TYPE* path = (const FS_CHAR_TYPE*)payload->Data;
				std::filesystem::path texturePath = Project::GetAssetDirectory() / path;

				if (AssetManager::IsExtensionValid(texturePath.extension().string(), AssetType::Texture))
				{
					mPanelData.texturePath = texturePath.string();
					mPanelData.sheetName = "";
					mTileWidth = 0; mTileHeight = 0;

					static int tileWidth = 0, tileHeight = 0;
					ModalManager::Open("New Spritesheet...", ModalButtons::OKCancel, 
					[this, texturePath]() 
					{
						ImGui::Text("Please enter the width and height of each tile in the sprite sheet:");
						ImGui::NewLine();

						StaticString<256> buffer(mPanelData.sheetName);
						if (ImGui::InputText("Name", buffer, buffer.length()))
							mPanelData.sheetName = buffer.toString();

						ImGui::InputInt("Width", &tileWidth);
						ImGui::InputInt("Height", &tileHeight);
					}, 
					[this, texturePath]() 
					{
						if (tileWidth > 0 && tileHeight > 0)
						{
							mPanelData.currentSheet = AssetManager::CreateNewAsset<Texture2DSheet>(mPanelData.sheetName + ".lss", "spritesheets/" + mPanelData.sheetName, texturePath.string(), glm::vec2{ tileWidth, tileHeight }, mPanelData.sheetName);
							mPanelData.framebuffer->resize(Cast<size_t>(mPanelData.viewportSize.x) - 15, 200);
						}
					});
				}
				else if (AssetManager::IsExtensionValid(texturePath.extension().string(), AssetType::TextureSheet))
				{
					mPanelData.currentSheet = AssetManager::GetAsset<Texture2DSheet>(texturePath);
					mPanelData.currentSubTex = nullptr;
					mPanelData.subTexName = noSubTex;
					mPanelData.framebuffer->resize(Cast<size_t>(mPanelData.viewportSize.x) - 15, 200);
				}

			}
			ImGui::EndDragDropTarget();
		}

		if (ImGui::Button("Add SubTex") && mPanelData.currentSheet)
			ModalManager::Open<SubTexModal>("SubTexModal", ModalButtons::OKCancel, mPanelData);

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
				for (const auto& [key, handle] : mPanelData.currentSheet->getSubTexList())
				{
					Ref<SubTexture2D> subtex = AssetManager::GetAsset<SubTexture2D>(handle);
					bool isSelected = mPanelData.subTexName == subtex->getName();

					if (ImGui::Selectable(subtex->getName().c_str(), isSelected))
					{
						mPanelData.subTexName = subtex->getName();
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
		else ImGui::Image((ImTextureID)(intptr_t)EditorResources::NoTexture->getRendererID(), { mPanelData.viewportSize.x - 15.0f, 200.0f }, { 0, 1 }, { 1, 0 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const FS_CHAR_TYPE* path = (const FS_CHAR_TYPE*)payload->Data;
				std::filesystem::path texturePath = Project::GetAssetDirectory() / path;

				if (AssetManager::IsExtensionValid(texturePath.extension().string(), AssetType::SubTexture))
				{
					mPanelData.currentSubTex = AssetManager::GetAsset<SubTexture2D>(texturePath);
					mPanelData.currentSheet = mPanelData.currentSubTex->getSheet();
					mPanelData.subTexName = mPanelData.currentSubTex->getName();
				}

			}
			ImGui::EndDragDropTarget();
		}

		ImGui::End();
	}
}