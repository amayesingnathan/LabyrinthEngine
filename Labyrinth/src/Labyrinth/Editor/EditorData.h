#pragma once

#include <Labyrinth/IO/Filesystem.h>
#include <Labyrinth/Renderer/EditorCamera.h>
#include <Labyrinth/Scene/Entity.h>

namespace Laby {

	struct EditorData
	{
		std::string currentFile;
		bool displayColliders = false;
		glm::vec4 colliderColour = { 0, 1, 0, 1 };
		glm::vec4 selectionColour = { 1, 0.4f, 0, 1 };

		bool viewportFocused = false, viewportHovered = false;
		glm::vec2 viewportSize = { 0.0f, 0.0f };
		std::array<glm::vec2, 2> viewportBounds = {};

		std::string newProjectName;
		fs::path newProjectFilepath;

		Ref<EditorCamera> camera = nullptr;
		Entity hoveredEntity;

		i32 gizmoType = -1;
	};
}