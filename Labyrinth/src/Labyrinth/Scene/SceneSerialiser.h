#pragma once

#include "Labyrinth/Scene/Scene.h"

namespace YAML {
	class Emitter;
	class Node;
}

namespace Laby{

	class Entity;

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

	public:
		static void SerializeEntity(YAML::Emitter& out, Entity entity);
		static void DeserializeEntities(YAML::Node& entitiesNode, Ref<Scene> scene);

	private:
		Ref<Scene> mScene;
	};
}