#pragma once

#include "EditorPanel.h"

#include "Labyrinth/Core/System/Log.h"

#include <string>
#include <unordered_map>

namespace Labyrinth {

	struct PanelItem
	{
		std::string key;
		Ref<EditorPanel> panel = nullptr;
		bool displayed = false;

		PanelItem() = default;
		PanelItem(const std::string& _key, const Ref<EditorPanel>& _panel, bool _display)
			: key(_key), panel(_panel), displayed(_display) {}
	};

	class PanelManager
	{
	private:
		inline static std::vector<PanelItem> sPanels;

		friend class EditorLayer;

	private:
		static const std::vector<PanelItem>& GetPanels() { return sPanels; }

		static auto Find(const std::string& key)
		{
			return std::find_if(sPanels.begin(), sPanels.end(), [&key](const PanelItem& panel) { return key == panel.key; });
		}

	public:
		static PanelItem* GetPanelItem(const std::string& key)
		{
			auto it = Find(key);
			if (it == sPanels.end())
				return nullptr;

			return &(*it);
		}

		static Ref<EditorPanel> Get(const std::string& name)
		{
			auto it = Find(name);
			if (it == sPanels.end())
			{
				LAB_WARN("EditorPanel was not registered with manager");
				return nullptr;
			}

			return it->panel;
		}

		template<typename T>
		static Ref<T> Get(const std::string& name)
		{
			return CastRefToRelative<T>(Get(name));
		}


		// Panels created are heap allocated, and must be cleaned up using the Clear() or Delete(const std::string&) functions. 
		template<typename T>
		static Ref<T> Register(const std::string& name, bool display = true)
		{
			static_assert(IsDerivedFrom<EditorPanel, T>());

			LAB_ASSERT(Find(name) == sPanels.end(), "Can't register panel that is already being managed! (Check name is not already in use)");

			Ref<T> newPanel = T::Create();
			sPanels.emplace_back(name, newPanel, display);

			return newPanel;
		}

		// Panels created are heap allocated, and must be cleaned up using the Clear() or Delete(const std::string&) functions. 
		template<typename T, typename... Args>
		static Ref<T> Register(const std::string& name, bool display, Args&&... args)
		{
			static_assert(IsDerivedFrom<EditorPanel, T>());

			LAB_ASSERT(Find(name) == sPanels.end(), "Can't register panel that is already being managed! (Check name is not already in use)");

			Ref<T> newPanel = T::Create(std::forward<Args>(args)...);
			sPanels.emplace_back(name, newPanel, display);

			return newPanel;
		}

		static void Delete(const std::string& name)
		{
			auto it = Find(name);
			if (it == sPanels.end())
			{
				LAB_ERROR("EditorPanel was not registered with manager");
				return;
			}

			sPanels.erase(it);
		}

		static void Clear() { sPanels.clear(); }

		static void UpdatePanels()
		{
			for (PanelItem& panelItem : sPanels)
			{
				if (panelItem.panel) panelItem.panel->onUpdate();
			}
		}

		static void RenderPanels()
		{
			for (PanelItem& panelItem : sPanels)
			{
				if (panelItem.panel && panelItem.displayed) panelItem.panel->onImGuiRender();
			}
		}

		static void DispatchEvents(Event& e)
		{
			for (PanelItem& panelItem : sPanels)
			{
				if (panelItem.panel) panelItem.panel->onEvent(e);
			}
		}

		static void ProjectChanged(Ref<Project> project)
		{
			for (PanelItem& panelItem : sPanels)
			{
				if (panelItem.panel) panelItem.panel->onProjectChange(project);
			}
		}
	};

}