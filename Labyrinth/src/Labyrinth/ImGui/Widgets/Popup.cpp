#include "Lpch.h"
#include "Popup.h"

#include <imgui.h>

namespace Laby::UI {

	PopUp::~PopUp()
	{
		if (!ImGui::BeginPopup(mStrID.data()))
			return;

		for (const PopUpItem& item : mPopUpItems)
		{
			if (ImGui::MenuItem(item.label.data()))
			{
				item.action();
				ImGui::CloseCurrentPopup();
			}
		}

		ImGui::EndPopup();
	}

	void PopUp::addPopUpItem(std::string_view label, Action<> action)
	{
		mPopUpItems.emplace_back(label, action);
	}

	PopUpContext::~PopUpContext()
	{
		if (!ImGui::BeginPopupContextItem())
			return;

		for (const PopUpItem& item : mPopUpItems)
		{
			if (ImGui::MenuItem(item.label.data()))
				item.action();
		}

		ImGui::EndPopup();
	}

	void PopUpContext::addPopUpItem(std::string_view label, Action<> action)
	{
		mPopUpItems.emplace_back(label, action);
	}
}
