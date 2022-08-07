#pragma once

#include "Labyrinth/Core/System/Base.h"
#include "Labyrinth/Core/System/Log.h"
#include "Labyrinth/Events/Event.h"
#include "Labyrinth/Project/Project.h"

#include <string>
#include <unordered_map>

namespace Labyrinth {

	class Panel : public RefCounted
	{
	public:
		virtual ~Panel() {}

		virtual void onUpdate() {};
		virtual void onImGuiRender() = 0;
		virtual void onEvent(Event& e) {}
		virtual void onProjectChange(Ref<Project> project) {}
	};

	struct PanelItem
	{
		std::string key;
		Ref<Panel> panel = nullptr;
		bool displayed = false;

		PanelItem() = default;
		PanelItem(const std::string& _key, const Ref<Panel>& _panel, bool _display)
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

	private:
		static auto Find(const std::string& key)
		{
			std::vector<PanelItem>& panels = GetPanels();
			return std::find_if(panels.begin(), panels.end(), [&key](const PanelItem& panel) { return key == panel.key; });
		}

	public:
		static std::vector<PanelItem>& GetPanels()
		{
			static PanelManager mInstance;
			return mInstance.mPanels;
		}

		static PanelItem* GetPanelItem(const std::string& key)
		{
			std::vector<PanelItem>& panels = GetPanels();

			auto it = Find(key);
			if (it == panels.end())
				return nullptr;

			return &(*it);
		}

		static Ref<Panel> Get(const std::string& name)
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
			static_assert(IsDerivedFrom<Panel, T>());

			std::vector<PanelItem>& panels = GetPanels();
			LAB_ASSERT(Find(name) == panels.end(), "Can't register panel that is already being managed! (Check name is not already in use)");

			Ref<T> newPanel = T::Create();
			panels.emplace_back(name, newPanel, display);

			return newPanel;
		}

		// Panels created are heap allocated, and must be cleaned up using the Clear() or Delete(const std::string&) functions. 
		template<typename T, typename... Args>
		static Ref<T> Register(const std::string& name, bool display, Args&&... args)
		{
			static_assert(IsDerivedFrom<Panel, T>());

			std::vector<PanelItem>& panels = GetPanels();
			LAB_ASSERT(Find(name) == panels.end(), "Can't register panel that is already being managed! (Check name is not already in use)");

			Ref<T> newPanel = T::Create(std::forward<Args>(args)...);
			panels.emplace_back(name, newPanel, display);

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

		static void ProjectChanged(Ref<Project> project)
		{
			for (PanelItem& panelItem : GetPanels())
			{
				if (panelItem.panel) panelItem.panel->onProjectChange(project);
			}
		}
	};

}