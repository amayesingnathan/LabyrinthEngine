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
		bool linkOnDestroy = false;

		EditorCamera camera;
		Entity hoveredEntity;

		std::string projectName;
		fs::path projectFilepath;
	};
}