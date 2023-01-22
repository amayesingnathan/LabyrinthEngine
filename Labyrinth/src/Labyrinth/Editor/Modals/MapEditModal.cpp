#include "Lpch.h"
#include "MapEditModal.h"

#include <Labyrinth/IO/Input.h>

namespace Laby {

    MapEditModal::MapEditModal(const Ref<Tilemap>& map)
        : mTilemap(TilemapUtils::Clone(map)), mMapWidth(map->getWidth()), mMapHeight(map->getHeight())
    {
    }

    void MapEditModal::onImGuiRender()
    {
    }

    void MapEditModal::onComplete()
    {
    }

    void MapEditModal::onEvent(Event& e)
    {
        LocalEventDispatcher dispatcher(e);
        dispatcher.dispatch<KeyPressedEvent>(LAB_BIND_EVENT_FUNC(MapEditModal::OnKeyPressed));
    }

    void MapEditModal::DrawMap()
    {
    }

    void MapEditModal::DrawSheet()
    {
    }

    bool MapEditModal::OnKeyPressed(KeyPressedEvent& e)
    {
        if (e.repeat)
            return false;

        bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);

        switch (e.keyCode)
        {
        case Key::Q:
        {
            if (control)
            {
                mEditMode = EditMode::Paint;
                return true;
            }
            break;
        }
        case Key::W:
        {
            if (control)
            {
                mEditMode = EditMode::Physics;
                return true;
            }
            break;
        }
        case Key::E:
        {
            if (control)
            {
                mEditMode = EditMode::Scripts;
                return true;
            }
            break;
        }
        }

        return false;
    }
}