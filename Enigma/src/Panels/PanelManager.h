#pragma once

#include "Labyrinth/Core/System/Log.h"

#include <string>
#include <unordered_map>

namespace Labyrinth {

	class Panel
	{
	public:
		virtual void onUpdate() {};
		virtual void onImGuiRender() = 0;
	};

	struct PanelItem
	{
		std::string key;
		Panel* panel = nullptr;
		bool displayed = false;

		PanelItem() = default;
		PanelItem(const std::string& _key, Panel* _panel, bool _display)
			: key(_key), panel(_panel), displayed(_display) {}
	};

	class PanelManager
	{
	private:
		PanelManager() = default;
		PanelManager(const PanelManager&) = delete;
		void operator=(const PanelManager&) = delete;

	private:
		std::vector<PanelItem> mPanels;

	public:
		static std::vector<PanelItem>& GetPanels()
		{
			static PanelManager mInstance;
			return mInstance.mPanels;
		}

		static auto Find(const std::string& key)
		{
			std::vector<PanelItem>& panels = GetPanels();
			return std::find_if(panels.begin(), panels.end(), [&key](const PanelItem& panel) { return key == panel.key; });
		}

		static Panel* Get(const std::string& name)
		{
			std::vector<PanelItem>& panels = GetPanels();
			auto it = Find(name);
			if (it == panels.end())
			{
				LAB_WARN("Panel was not registered with manager");
				return nullptr;
			}

			return it->panel;
		}

		template<typename T>
		static T* Get(const std::string& name)
		{
			return Cast<T>(Get(name));
		}


		// Panels created are heap allocated, and must be cleaned up using the Clear() or Delete(const std::string&) functions. 
		template<typename T>
		static T* Register(const std::string& name, bool display = true)
		{
			static_assert(IsDerivedFrom<Panel, T>());

			std::vector<PanelItem>& panels = GetPanels();
			LAB_ASSERT(Find(name) == panels.end(), "Can't register panel that is already being managed! (Check name is not already in use)");

			T* newPanel = new T;
			panels.emplace_back(name, newPanel, display);

			return newPanel;
		}

		// Panels created are heap allocated, and must be cleaned up using the Clear() or Delete(const std::string&) functions. 
		template<typename T, typename... Args>
		static T* Register(const std::string& name, bool display, Args&... args)
		{
			static_assert(IsDerivedFrom<Panel, T>());

			std::vector<PanelItem>& panels = GetPanels();
			LAB_ASSERT(Find(name) == panels.end(), "Can't register panel that is already being managed! (Check name is not already in use)");

			T* newPanel = new T(std::forward<Args>(args)...);
			panels.emplace_back(name, newPanel, display);

			return newPanel;
		}

		// Panel pointers passed must be heap allocated, and must be cleaned up using the Clear() or Delete(const std::string&) functions. 
		template<typename T>
		static T* Register(const std::string& name, T* panel, bool display = true)
		{
			static_assert(IsDerivedFrom<Panel, T>());

			std::vector<PanelItem>& panels = GetPanels();
			LAB_ASSERT(Find(name) == panels.end(), "Can't register panel that is already being managed! (Check name is not already in use)");

			panels.emplace_back(name, newPanel, display);

			return panel;
		}

		static void Delete(const std::string& name)
		{
			std::vector<PanelItem>& panels = GetPanels();

			auto it = Find(name);
			if (it == panels.end())
			{
				LAB_ERROR("Panel was not registered with manager");
				return;
			}

			delete it->panel;
			panels.erase(it);
		}

		static void Clear() 
		{
			std::vector<PanelItem>& panels = GetPanels();
			for (PanelItem& panelItem : panels)
				delete panelItem.panel;

			panels.clear();
		}

		static void UpdatePanels()
		{
			for (PanelItem& panelItem : GetPanels())
			{
				if (panelItem.panel) panelItem.panel->onUpdate();
			}
		}

		static void RenderPanels()
		{
			for (PanelItem& panelItem : GetPanels())
			{
				if (panelItem.panel && panelItem.displayed) panelItem.panel->onImGuiRender();
			}
		}
	};

}