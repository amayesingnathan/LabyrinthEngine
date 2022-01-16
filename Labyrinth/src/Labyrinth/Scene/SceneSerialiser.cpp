#include "Lpch.h"
#include "SceneSerialiser.h"

#include "Entity.h"
#include "Components.h"

#include "Labyrinth/IO/YAML.h"

#include <fstream>



namespace Labyrinth {

	SceneSerialiser::SceneSerialiser(const Ref<Scene>& scene)
		: mScene(scene)
	{
	}


	void SceneSerialiser::serialise(const std::string& filepath)
	{
		YAMLParser parser;
		parser.serialiseObject<Scene>(*mScene.get());

		std::ofstream fout(filepath);
		fout << parser.getData();
	}

	void SceneSerialiser::serialiseRuntime(const std::string& filepath)
	{
		// Not implemented
		LAB_CORE_ASSERT(false);
	}

	bool SceneSerialiser::deserialise(const std::string& filepath)
	{
		YAMLParser parser(filepath);
		return parser.deserialiseScene(mScene);
	}

	bool SceneSerialiser::deserialiseRuntime(const std::string& filepath)
	{
		// Not implemented
		LAB_CORE_ASSERT(false);
		return false;
	}


}