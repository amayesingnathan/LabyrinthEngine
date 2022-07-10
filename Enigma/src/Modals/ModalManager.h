#pragma once

#include "Modal.h"

namespace Labyrinth {

	class ModalManager
	{
	public:
		struct ModalEntry
		{
			Ref<Modal> modal = nullptr;
			bool begin = true;
			ImGuiWindowFlags flags = ImGuiWindowFlags_None;

			ModalEntry(const Ref<Modal>& _modal, ImGuiWindowFlags _flags) : modal(_modal), flags(_flags) {}
		};

	private:
		ModalManager() = default;
		ModalManager(const ModalManager&) = delete;
		void operator=(const ModalManager&) = delete;

	private:
		using ModalData = std::unordered_map<std::string, ModalEntry>;
		ModalData mModals;

		friend class EditorLayer;

	public:
		static ModalData& GetModals()
		{
			static ModalManager mInstance;
			return mInstance.mModals;
		}
		template<typename ModalType, typename... Args>
		static void Open(const std::string& name, ImGuiWindowFlags flags, Args&&... args)
		{
			ModalData& modals = GetModals();
			if (modals.count(name) != 0)
			{
				LAB_WARN("Modal already exists!");
				return;
			}

			modals.try_emplace(name, CastRefToRelative<Modal>(ModalType::Create(std::forward<Args>(args)...)), flags);
		}

		template<typename ModalType, typename... Args>
		static void Open(const std::string& name, Args&&... args)
		{
			ModalData& modals = GetModals();
			if (modals.count(name) != 0)
			{
				LAB_WARN("Modal already exists!");
				return;
			}

			modals.try_emplace(name, ModalType::Create(std::forward<Args>(args)...), ImGuiWindowFlags_None);
		}

	private:
		static void Display()
		{
			ModalData& modals = GetModals();

			std::vector<std::string> toRemove;
			toRemove.reserve(modals.size());

			for (auto& [key, modalData] : modals)
			{
				if (!modalData.modal) continue;

				if (modalData.begin)
				{
					ImGui::OpenPopup(key.c_str());
					modalData.begin = false;
				}

				ImVec2 centre = ImGui::GetMainViewport()->GetCenter();
				ImGui::SetNextWindowPos(centre, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

				if (modalData.modal->complete() || !ImGui::BeginPopupModal(key.c_str(), nullptr, modalData.flags))
				{
					toRemove.push_back(key);
					continue;
				}

				modalData.modal->display();
				ImGui::EndPopup();
			}

			for (const std::string& key : toRemove)
				modals.erase(key);
		}

	};

}