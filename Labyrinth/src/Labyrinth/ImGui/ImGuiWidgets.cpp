#include "Lpch.h"
#include "ImGuiWidgets.h"

#include <Labyrinth/Editor/EditorResources.h>

using imcpp::Widgets;
using imcpp::Utils;

namespace Laby {

	void LabWidgets::Image(Ref<IRenderable> image, const glm::vec2& size, float rotation)
	{
		if (!image)
			image = EditorResources::NoTexture;

		const glm::vec2* coords = image->getTextureCoords();
		Widgets::Image(image->getTextureID(), size, rotation, coords[3], coords[1]);
	}

	void LabWidgets::ImageButton(Ref<IRenderable> image, const glm::vec2& size, Action<> action)
	{
		if (!image)
			image = EditorResources::NoTexture;

		const glm::vec2* coords = image->getTextureCoords();
		Widgets::ImageButton(image->getTextureID(), size, action, coords[3], coords[1]);
	}

	bool LabWidgets::ComponentImpl(void* id, std::string_view text, bool selected, ImGuiTreeNodeFlags flags, Action<> whileOpen)
	{
		flags |= selected ? ImGuiTreeNodeFlags_Selected : 0;
		bool open = Widgets::TreeNodeEx(id, text, flags);
		bool removeComponent = false;

		f32 lineHeight = Utils::LineHeight();
		Widgets::SameLine(Utils::WindowWidth() - lineHeight);
		Widgets::Button(glm::vec2{ lineHeight }, "+", []() { Widgets::OpenPopup("ComponentSettings"); });

		Widgets::BeginPopup("ComponentSettings");
		Widgets::AddPopupItem("Remove component", [&]() { removeComponent = true; });
		Widgets::EndPopup();

		if (open)
		{
			whileOpen();
			ImGui::TreePop();
		}

		return removeComponent;
	}
}

