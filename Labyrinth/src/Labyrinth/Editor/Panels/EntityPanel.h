#pragma once

#include <Labyrinth/Editor/IEditorPanel.h>
#include <Labyrinth/ImGui/ImGuiWidgets.h>
#include <Labyrinth/Renderer/Framebuffer.h>
#include <Labyrinth/Scene/Scene.h>
#include <Labyrinth/Scene/Entity.h>
#include <Labyrinth/Scene/Components.h>

typedef int ImGuiSliderFlags;

namespace Laby {

	class EntityPanel : public IEditorPanel
	{
	public:
		EntityPanel(const Ref<Scene>& scene);

		void SetContext(const Ref<Scene>& scene) { mContext = scene; }

		void OnRender() override;
		void OnSelectionChange() override;

	private:
		void DrawComponents();

		template<typename T>
		void DrawAddComponentEntry(std::string_view text)
		{
			if (mSelectedEntity.HasComponent<T>())
				return;

			Widgets::AddPopupItem(text, [&]() { mSelectedEntity.AddComponent<T>(); });
		}

		void DrawChildControllerElement(const std::string& name, glm::vec3& componentElement, glm::vec3& displayElement, glm::vec3& lastDisplay, float min, float max, ImGuiSliderFlags flags = 0);
		void DrawScriptClassFields(Entity entity);
		void DrawScriptInstanceFields(Entity entity);

	private:
		Ref<Scene> mContext = nullptr;

		Entity mSelectedEntity;
		Entity mPreviousEntity;
	};

}