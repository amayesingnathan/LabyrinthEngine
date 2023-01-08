#pragma once

#include <Labyrinth/Core/System/Base.h>

namespace Laby {

	class Widgets;

	namespace UI {

		enum class MenuItemType
		{
			Action, Switch, Separator
		};

		struct MenuItem
		{
			MenuItemType type = MenuItemType::Action;
			std::string_view label;
			std::string_view shortcut = nullptr;
			Action<> action;
			bool* display = nullptr;

			MenuItem(MenuItemType itemType) : type(itemType) {}
			MenuItem(MenuItemType itemType, std::string_view heading, std::string_view key, Action<> delegate)
				: type(itemType), label(heading), shortcut(key), action(delegate) {}
			MenuItem(MenuItemType itemType, std::string_view heading, std::string_view key, bool& show)
				: type(itemType), label(heading), shortcut(key), display(&show) {}
		};

		struct MenuHeading
		{
			std::string_view label;
			std::vector<MenuItem> menu;

			MenuHeading(std::string_view text)
				: label(text) {}
		};

		class MenuBar
		{
		public:
			~MenuBar();

		private:
			void addHeading(std::string_view heading);
			void addMenuItemAction(std::string_view label, std::string_view shortcut, Action<> action);
			void addMenuItemSwitch(std::string_view label, std::string_view shortcut, bool& show);
			void addSeparator();

		private:
			std::vector<MenuHeading> mMenuItems;

			friend Widgets;
		};
	}
}