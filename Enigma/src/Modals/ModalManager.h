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
			std::function<void()> onComplete = std::function<void()>();

			ModalEntry(const Ref<Modal>& _modal, ImGuiWindowFlags _flags, std::function<void()> _onComplete) : modal(_modal), flags(_flags), onComplete(_onComplete) {}
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
		static void Open(const std::string& name, ImGuiWindowFlags flags, std::function<void()> onComplete, Args&&... args)
		{
			ModalData& modals = GetModals();
			if (modals.count(name) != 0)
			{
				LAB_WARN("Modal already exists!");
				return;
			}

			modals.try_emplace(name,ModalType::Create(std::forward<Args>(args)...), flags, onComplete);
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

				modalData.modal->onImGuiRender();
				ImGui::EndPopup();
			}

			for (const std::string& key : toRemove)
			{
				modals.at(key).onComplete();
				modals.erase(key);
			}
		}

	};

}