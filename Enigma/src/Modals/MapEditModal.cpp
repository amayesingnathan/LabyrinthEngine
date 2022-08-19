#include "MapEditModal.h"

#include "../Panels/ScenePanel.h"

#include <Labyrinth/Assets/AssetManager.h>
#include <Labyrinth/Editor/ModalManager.h>
#include <Labyrinth/Editor/SelectionManager.h>

#include <glm/gtc/type_ptr.hpp>

namespace Labyrinth {

    MapEditModal::MapEditModal(const Ref<Tilemap>& map) : EditorModal(), mTilemap(map), mMapWidth(map->getWidth()), mMapHeight(map->getHeight())
    {
    }

    void MapEditModal::onImGuiRender()
    {
		float xpos = ImGui::GetCursorPosX();
		float ypos = ImGui::GetCursorPosY();

		auto imageSize = ImGui::GetWindowSize();
		imageSize = { imageSize.x - 2 * xpos, imageSize.y - 40.f - (1.5f * ypos) };
		ImVec2 tileSize = { imageSize.x / mMapWidth, imageSize.y / mMapHeight };
		ImGui::Image((ImTextureID)(uintptr_t)mTilemap->getTex()->getRendererID(), {imageSize.x, imageSize.y}, {0, 0}, {1, 1});

		for (size_t y = 0; y < mMapHeight; y++)
		{
			for (size_t x = 0; x < mMapWidth; x++)
			{
				std::string name = fmt::format("##Tile({}, {})", x, y);

				ImGui::SetCursorPosX(xpos + (y * tileSize.x));
				ImGui::SetCursorPosY(ypos + (x * tileSize.y));

				ImGui::Button(name.c_str(), tileSize);
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MAP_EDIT_TEXTURE_ITEM"))
					{



					}
					ImGui::EndDragDropTarget();
				}
			}
		}

    }

    void MapEditModal::onComplete()
    {
    }
}
