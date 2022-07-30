#pragma once

#include "Scene.h"

#include "Labyrinth/Renderer/SubTexture.h"

namespace Labyrinth {

	class SceneSerialiser
	{
	public:
		SceneSerialiser(const Ref<Scene>& scene);

		void serialise(const std::filesystem::path& filepath);
		void serialiseRuntime(const std::filesystem::path& filepath)
		{
			// Not implemented
			LAB_CORE_ASSERT(false);
		}

		bool deserialise(const std::filesystem::path& filepath);
		bool deserialiseRuntime(const std::filesystem::path& filepath)
		{
			// Not implemented
			LAB_CORE_ASSERT(false);
			return false;
		}

	private:
		Ref<Scene> mScene;
	};
}