#pragma once

#include "IEditorPanel.h"

namespace Laby {

	class PanelManager
	{
	private:
		struct PanelEntry
		{
			std::string_view key;
			Ref<IEditorPanel> panel = nullptr;
			bool displayed = false;

			PanelEntry() = default;
			PanelEntry(std::string_view _key, const Ref<IEditorPanel>& _panel, bool _display)
				: key(_key), panel(_panel), displayed(_display) {}
		};

	private:
		static std::vector<PanelEntry>& GetPanels() { return sEditorPanels; }

		static void Update(Timestep ts);
		static void Render();
		static void DispatchEvents(Event& e);
		static void ProjectChanged(Ref<Project> project);
		static void SelectionChange();

		static bool Contains(std::string_view key) { return Find(key) != sEditorPanels.end(); }
		static std::vector<PanelEntry>::iterator Find(std::string_view key) { return std::find_if(sEditorPanels.begin(), sEditorPanels.end(), [&key](const PanelEntry& panel) { return key == panel.key; }); }

		static void Delete(std::string_view key);
		static void Clear() { sEditorPanels.clear(); }

		template<IsEditorPanel T>
		static Ref<T> Register(std::string_view key, bool display = true)
		{
			LAB_ASSERT(!Contains(key), "Can't register panel that is already being managed! (Check name is not already in use)");

			Ref<T> newPanel = Ref<T>::Create();
			sEditorPanels.emplace_back(key, newPanel, display);

			return newPanel;
		}

		template<IsEditorPanel T, typename... Args>
		static Ref<T> Register(std::string_view key, bool display, Args&&... args)
		{
			LAB_ASSERT(!Contains(key), "Can't register panel that is already being managed! (Check name is not already in use)");

			Ref<T> newPanel = Ref<T>::Create(std::forward<Args>(args)...);
			sEditorPanels.emplace_back(key, newPanel, display);

			return newPanel;
		}

	private:
		inline static std::vector<PanelEntry> sEditorPanels;

		friend class EditorLayer;
		friend class SelectionManager;
	};
}