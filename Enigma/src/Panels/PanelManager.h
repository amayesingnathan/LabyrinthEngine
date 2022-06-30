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
		Panel* panel = nullptr;
		bool displayed = false;

		PanelItem() = default;
		PanelItem(Panel* _panel, bool _display)
			: panel(_panel), displayed(_display) {}
		~PanelItem() { if (panel) delete panel; }
	};

	class PanelManager
	{
	private:
		PanelManager() = default;
		PanelManager(const PanelManager&) = delete;
		void operator=(const PanelManager&) = delete;

	private:
		using PanelData = std::unordered_map<std::string, PanelItem>;
		PanelData mPanels;

	public:
		static PanelData& GetPanels()
		{
			static PanelManager mInstance;
			return mInstance.mPanels;
		}

		static Panel* Get(const std::string& name)
		{
			PanelData& panels = GetPanels();

			if (panels.count(name) == 0)
			{
				LAB_WARN("Panel was not registered with manager");
				return nullptr;
			}

			return panels[name].panel;
		}

		template<typename T>
		static T* Get(const std::string& name)
		{
			return Cast<T>(Get(name));
		}

		template<typename T>
		static T* Register(const std::string& name, bool display = true)
		{
			static_assert(IsDerivedFrom<Panel, T>());

			PanelData& panels = GetPanels();
			LAB_ASSERT(panels.count(name) == 0, "Can't register panel that is already being managed! (Check name is not already in use)");

			T* newPanel = new T;
			panels.try_emplace(name, newPanel, display);

			return newPanel;
		}

		// Panel pointers passed must be heap allocated, and will have their lifetime handled by the panel manager.
		template<typename T>
		static T* Register(const std::string& name, T* panel, bool display = true)
		{
			static_assert(IsDerivedFrom<Panel, T>());

			PanelData& panels = GetPanels();
			LAB_ASSERT(panels.count(name) == 0, "Can't register panel that is already being managed! (Check name is not already in use)");

			panels.try_emplace(name, newPanel, display);

			return panel;
		}

		static void Delete(const std::string& name)
		{
			PanelData& panels = GetPanels();

			if (panels.count(name) == 0)
			{
				LAB_WARN("Panel was not registered with manager");
				return;
			}

			panels.erase(name);
		}

		static void Clear() { GetPanels().clear(); }

		static void UpdatePanels()
		{
			for (auto& [key, panel] : GetPanels())
			{
				if (panel.panel) panel.panel->onUpdate();
			}
		}

		static void RenderPanels()
		{
			for (auto& [key, panel] : GetPanels())
			{
				if (panel.panel && panel.displayed) panel.panel->onImGuiRender();
			}
		}
	};

}