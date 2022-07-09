#pragma once

#include "Labyrinth/Core/System/Base.h"
#include "Labyrinth/Core/System/Log.h"
#include "Labyrinth/Events/Event.h"

#include <string>
#include <unordered_map>

namespace Labyrinth {

	class IPanel
	{
	public:
		virtual ~IPanel() {}

		virtual void onUpdate() {};
		virtual void onImGuiRender() = 0;
		virtual void onEvent(Event& e) {}
	};

	struct PanelItem
	{
		std::string key;
		Ref<IPanel> panel = nullptr;
		bool displayed = false;

		PanelItem() = default;
		PanelItem(const std::string& _key, const Ref<IPanel>& _panel, bool _display)
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

		static Ref<IPanel> Get(const std::string& name)
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
		static Ref<T> Get(const std::string& name)
		{
			return CastRefToRelative<T>(Get(name));
		}


		// Panels created are heap allocated, and must be cleaned up using the Clear() or Delete(const std::string&) functions. 
		template<typename T>
		static Ref<T> Register(const std::string& name, bool display = true)
		{
			static_assert(IsDerivedFrom<IPanel, T>());

			std::vector<PanelItem>& panels = GetPanels();
			LAB_ASSERT(Find(name) == panels.end(), "Can't register panel that is already being managed! (Check name is not already in use)");

			Ref<T> newPanel = T::Create();
			panels.emplace_back(name, CastRefToRelative<IPanel>(newPanel), display);

			return newPanel;
		}

		// Panels created are heap allocated, and must be cleaned up using the Clear() or Delete(const std::string&) functions. 
		template<typename T, typename... Args>
		static Ref<T> Register(const std::string& name, bool display, Args&... args)
		{
			static_assert(IsDerivedFrom<IPanel, T>());

			std::vector<PanelItem>& panels = GetPanels();
			LAB_ASSERT(Find(name) == panels.end(), "Can't register panel that is already being managed! (Check name is not already in use)");

			Ref<T> newPanel = T::Create(std::forward<Args>(args)...);
			panels.emplace_back(name, CastRefToRelative<IPanel>(newPanel), display);

			return newPanel;
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

			panels.erase(it);
		}

		static void Clear() { GetPanels().clear(); }

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

		static void DispatchEvents(Event& e)
		{
			for (PanelItem& panelItem : GetPanels())
			{
				if (panelItem.panel) panelItem.panel->onEvent(e);
			}
		}
	};

}