#include "Lpch.h"
#include "Serialiser.h"

#include "Entity.h"
#include "Components.h"

#include <fstream>


namespace YAML {

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			//node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			//node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};

}


namespace Labyrinth {

	YAML::Emitter& operator<<(YAML::Emitter& mOut, const glm::vec3& v)
	{
		mOut << YAML::Flow;
		mOut << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return mOut;
	}

	YAML::Emitter& operator<<(YAML::Emitter& mOut, const glm::vec4& v)
	{
		mOut << YAML::Flow;
		mOut << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return mOut;
	}

	template<>
	void YAMLParser::encodeObject<TagComponent>(const TagComponent& tag)
	{
		BeginObject("TagComponent");
		ObjectProperty("Tag", tag.tag);
		EndObject();
	}

	template<>
	void YAMLParser::encodeObject<TransformComponent>(const TransformComponent& transform)
	{
		BeginObject("TransformComponent");

		ObjectProperty("Translation", transform.translation);
		ObjectProperty("Rotation", transform.translation);
		ObjectProperty("Scale", transform.translation);

		EndObject();
	}

	template<>
	void YAMLParser::encodeObject<CameraComponent>(const CameraComponent& cameraComponent)
	{
		BeginObject("CameraComponent");

		auto& camera = cameraComponent.camera;

		BeginObject("Camera");
		ObjectProperty("ProjectionType", (int)camera.getProjectionType());
		ObjectProperty("PerspectiveFOV", camera.getPerspectiveVerticalFOV());
		ObjectProperty("PerspectiveNear", camera.getPerspectiveNearClip());
		ObjectProperty("PerspectiveFar", camera.getPerspectiveFarClip());
		ObjectProperty("OrthographicSize", camera.getOrthographicSize());
		ObjectProperty("OrthographicNear", camera.getOrthographicNearClip());
		ObjectProperty("OrthographicFar", camera.getOrthographicFarClip());
		EndObject();

		ObjectProperty("Primary", cameraComponent.primary);
		ObjectProperty("FixedAspectRatio", cameraComponent.fixedAspectRatio);

		EndObject();
	}

	template<>
	void YAMLParser::encodeObject<SpriteRendererComponent>(const SpriteRendererComponent& srComponent)
	{
		BeginObject("SpriteRendererComponent");

		ObjectProperty("Colour", srComponent.colour);

		mOut << YAML::EndMap; // SpriteRendererComponent
	}

	template<>
	void YAMLParser::encodeObject<Entity>(const Entity& entity)
	{
		BeginObject();
		ObjectProperty("Entity", entity.getID());

		if (entity.hasComponent<TagComponent>())
		{
			const TagComponent& tc = entity.getComponent<TagComponent>();
			encodeObject<TagComponent>(tc);
		}

		if (entity.hasComponent<TransformComponent>())
		{
			const TransformComponent& tc = entity.getComponent<TransformComponent>();
			encodeObject<TransformComponent>(tc);
		}

		if (entity.hasComponent<CameraComponent>())
		{
			const CameraComponent& cc = entity.getComponent<CameraComponent>();
			encodeObject<CameraComponent>(cc);
		}

		if (entity.hasComponent<SpriteRendererComponent>())
		{
			const SpriteRendererComponent& src = entity.getComponent<SpriteRendererComponent>();
			encodeObject<SpriteRendererComponent>(src);
		}

		EndObject();
	}

	template<>
	void YAMLParser::encodeObject<Ref<Scene>>(const Ref<Scene>& scene)
	{
		BeginObject();
		ObjectProperty("Scene", "Untitled");
		BeginSequence("Entities");

		scene->mRegistry.each([&](auto entityID)
			{
				Entity entity = { entityID, scene.get() };
				if (!entity)
					return;

				encodeObject(entity);
			});

		EndSequence();
		EndObject();
	}

	template<>
	TransformComponent* YAMLParser::decodeObject<TransformComponent>(Entity entity, YAML::Node node)
	{
		auto transformComponent = node["TransformComponent"];
		if (transformComponent)
		{
			// Entities always have transforms
			auto& tc = entity.getComponent<TransformComponent>();
			tc.translation = transformComponent["Translation"].as<glm::vec3>();
			tc.rotation = transformComponent["Rotation"].as<glm::vec3>();
			tc.scale = transformComponent["Scale"].as<glm::vec3>();
			return &tc;
		}
		return nullptr;
	}

	template<>
	CameraComponent* YAMLParser::decodeObject<CameraComponent>(Entity entity, YAML::Node node)
	{
		auto cameraComponent = node["CameraComponent"];
		if (cameraComponent)
		{
			auto& cc = entity.addComponent<CameraComponent>();

			auto& cameraProps = cameraComponent["Camera"];
			cc.camera.setProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());

			cc.camera.setPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
			cc.camera.setPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
			cc.camera.setPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

			cc.camera.setOrthographicSize(cameraProps["OrthographicSize"].as<float>());
			cc.camera.setOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
			cc.camera.setOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());

			cc.primary = cameraComponent["Primary"].as<bool>();
			cc.fixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
			return &cc;
		}
		return nullptr;
	}

	template<>
	SpriteRendererComponent* YAMLParser::decodeObject<SpriteRendererComponent>(Entity entity, YAML::Node node)
	{
		auto spriteRendererComponent = node["SpriteRendererComponent"];
		if (spriteRendererComponent)
		{
			auto& src = entity.addComponent<SpriteRendererComponent>();
			src.colour = spriteRendererComponent["Colour"].as<glm::vec4>();
			return &src;
		}
		return nullptr;
	}

	template<>
	Entity* YAMLParser::decodeObject<Entity, Ref<Scene>>(Ref<Scene> scene, YAML::Node entity)
	{
		uint64_t uuid = entity["Entity"].as<uint64_t>(); // TODO

		std::string name;
		auto tagComponent = entity["TagComponent"];
		if (tagComponent)
			name = tagComponent["Tag"].as<std::string>();

		LAB_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);

		Entity deserializedEntity = scene->CreateEntity(name);

		//Must add new components here as they are added.
		decodeObject<TransformComponent>(deserializedEntity, entity);
		decodeObject<CameraComponent>(deserializedEntity, entity);
		decodeObject<SpriteRendererComponent>(deserializedEntity, entity);

		return &deserializedEntity;
	}

	template<>
	Entity* YAMLParser::decodeObject<Entity, Ref<Scene>>(Ref<Scene> scene)
	{
		if (!mIn["Entity"]) return false;

		auto entity = mIn["Entity"];
		decodeObject<Entity>(scene, entity);

		return nullptr;
	}

	template<>
	Scene* YAMLParser::decodeObject<Scene, Ref<Scene>>(Ref<Scene> scene)
	{
		if (!mIn["Scene"]) return nullptr;

		bool success = true;

		std::string sceneName = mIn["Scene"].as<std::string>();
		LAB_CORE_TRACE("Deserializing scene '{0}'", sceneName);

		auto entities = mIn["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				Entity* deserialisedEntity = decodeObject<Entity>(scene, entity);
				if (!deserialisedEntity) return nullptr;
			}
		}

		return scene.get();
	}
























	SceneSerialiser::SceneSerialiser(const Ref<Scene>& scene)
		: mScene(scene)
	{
	}


	void SceneSerialiser::serialise(const std::string& filepath)
	{
		//YAMLParser parser;
		//parser.serialiseObject<Scene>(*mScene.get());

		//std::ofstream fout(filepath);
		//fout << parser.getData();
	}

	void SceneSerialiser::serialiseRuntime(const std::string& filepath)
	{
		// Not implemented
		LAB_CORE_ASSERT(false);
	}

	bool SceneSerialiser::deserialise(const std::string& filepath)
	{
		//YAMLParser parser(filepath);
		//return parser.deserialiseScene(mScene);
		return false;
	}

	bool SceneSerialiser::deserialiseRuntime(const std::string& filepath)
	{
		// Not implemented
		LAB_CORE_ASSERT(false);
		return false;
	}

}