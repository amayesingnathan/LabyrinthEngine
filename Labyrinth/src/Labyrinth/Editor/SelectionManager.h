#pragma once

#include <Labyrinth/Core/UUID.h>

namespace Laby {

    enum class SelectionDomain
    {
        Global, Scene
    };

    class SelectionManager
    {
    public:
        static void Select(SelectionDomain domain, UUID selection);

        static void Deselect(UUID selection);
        static void Deselect(SelectionDomain domain, UUID selection);

        static void DeselectAll();
        static void DeselectAll(SelectionDomain domain);

        static bool IsSelected(UUID selection);
        static bool IsSelected(SelectionDomain domain, UUID selection);

        static UUID GetSelection(SelectionDomain domain, usize index);
        static usize GetSelectionCount(SelectionDomain domain);

        static const std::vector<UUID>& GetSelections(SelectionDomain domain);

        static void ForEach(SelectionDomain domain, Action<UUID>&& func);
        static void ForEach(Action<UUID>&& func);

    private:
        inline static std::unordered_map<SelectionDomain, std::vector<UUID>> sSelectionMap;
    };
}