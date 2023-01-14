#pragma once

#include <Labyrinth/Core/System/Base.h>

namespace Laby {

	namespace UI {

		struct PopUpItem
		{
			std::string_view label;
			Action<> action;

			PopUpItem(std::string_view heading, Action<> delegate)
				: label(heading), action(delegate) {}
		};

		class PopUp
		{
		public:
			PopUp(std::string_view strID) : mStrID(strID) {}
			~PopUp();

		private:
			void addPopUpItem(std::string_view label, Action<> action);

		private:
			std::string_view mStrID;
			std::vector<PopUpItem> mPopUpItems;

			friend class Widgets;
		};

		class PopUpContext
		{
		public:
			~PopUpContext();

		private:
			void addPopUpItem(std::string_view label, Action<> action);

		private:
			std::vector<PopUpItem> mPopUpItems;

			friend class Widgets;
		};
	}
}