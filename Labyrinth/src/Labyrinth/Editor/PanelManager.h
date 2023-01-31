#pragma once

#include <ranges>

#include "IEditorPanel.h"

namespace Laby {

	class PanelManager
	{
	private:
		struct PanelEntry
		{
			std::string_view key;
			Ref<IEditorPanel> panel = nullptr;
			bool displayed = true;

			PanelEntry() = default;
			PanelEntry(std::string_view _key, const Ref<IEditorPanel>& _panel)
				: key(_key), panel(_panel) {}
		};

	private:
		static std::vector<PanelEntry>& GetPanels() { return sEditorPanels; }

		static void Render();
		static void ProjectChanged();
		static void SelectionChange();

		static PanelEntry* Find(std::string_view key);
		static bool Contains(std::string_view key);

		static void Delete(std::string_view key);
		static void Clear() { sEditorPanels.clear(); }

		template<IsEditorPanel T, typename... Args>
		static Ref<T> Register(std::string_view key, Args&&... args)
		{
			LAB_ASSERT(!Contains(key), "Can't register panel that is already being managed! (Check name is not already in use)");

			Ref<T> newPanel = Ref<T>::Create(std::forward<Args>(args)...);
			sEditorPanels.emplace_back(key, newPanel);

			return newPanel;
		}

	private:
		inline static std::vector<PanelEntry> sEditorPanels;

		friend class EditorLayer;
		friend class SelectionManager;
	};
}