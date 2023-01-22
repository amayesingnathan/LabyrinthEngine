#include "Lpch.h"
#include "StatisticsPanel.h"

#include <Labyrinth/ImGui/ImGuiWidgets.h>
#include <Labyrinth/Renderer/Renderer2D.h>
#include <Labyrinth/Scene/Entity.h>

namespace Laby {

	void StatisticsPanel::onImGuiRender()
	{
		Widgets::Label(std::format("Hovered Entity: {}", mHoveredEntity ? mHoveredEntity.getTag() : "None"));

		auto stats = Renderer2D::GetStats();
		Widgets::Label("Renderer2D Stats:");
		Widgets::Label(std::format("Draw Calls: {}", stats.drawCalls));
		Widgets::Label(std::format("Quads: {}", stats.quadCount));
		Widgets::Label(std::format("Vertices: {}", stats.getTotalVertexCount()));
		Widgets::Label(std::format("Indices: {}", stats.getTotalIndexCount()));
	}
}