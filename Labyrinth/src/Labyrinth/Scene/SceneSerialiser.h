#pragma once

#include "Labyrinth/Core/Base.h"

#include "Scene.h"

namespace Labyrinth {

	class SceneSerialiser
	{
	public:
		SceneSerialiser(const Ref<Scene>& scene);

		void serialise(const std::string& filepath);
		void serialiseRuntime(const std::string& filepath);

		bool deserialise(const std::string& filepath);
		bool deserialiseRuntime(const std::string& filepath);

	private:
		Ref<Scene> mScene;
	};

}