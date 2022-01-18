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
	Ref<TransformComponent> YAMLParser::decodeObject<TransformComponent>(Entity entity, YAML::Node node)
	{
		auto transformComponent = node["TransformComponent"];
		if (transformComponent)
		{
			// Entities always have transforms
			auto& tc = entity.getComponent<TransformComponent>();
			tc.translation = transformComponent["Translation"].as<glm::vec3>();
			tc.rotation = transformComponent["Rotation"].as<glm::vec3>();
			tc.scale = transformComponent["Scale"].as<glm::vec3>();
			return CreateRef<TransformComponent>(tc);
		}
		return nullptr;
	}

	template<>
	TransformComponent YAMLParser::decodeObject<TransformComponent>()
	{
		LAB_CORE_ASSERT(mIn["TransformComponent"], "File must contain a transform component!");

		auto& transformComponent = mIn["TransformComponent"];
		return TransformComponent(
			transformComponent["Translation"].as<glm::vec3>(), 
			transformComponent["Rotation"].as<glm::vec3>(), 
			transformComponent["Scale"].as<glm::vec3>()
		);
	}

	template<>
	Ref<CameraComponent> YAMLParser::decodeObject<CameraComponent>(Entity entity, YAML::Node node)
	{
		auto& cameraComponent = node["CameraComponent"];
		if (cameraComponent)
		{
			auto& cameraProps = cameraComponent["Camera"];
			auto& cc = entity.addComponent<CameraComponent>();
			cc.camera.setProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());

			cc.camera.setPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
			cc.camera.setPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
			cc.camera.setPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

			cc.camera.setOrthographicSize(cameraProps["OrthographicSize"].as<float>());
			cc.camera.setOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
			cc.camera.setOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());

			cc.primary = cameraComponent["Primary"].as<bool>();
			cc.fixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
			return CreateRef<CameraComponent>(cc);
		}
		return nullptr;
	}

	template<>
	CameraComponent YAMLParser::decodeObject<CameraComponent>()
	{
		LAB_CORE_ASSERT(mIn["CameraComponent"], "File must contain a camera component!");

		auto& cameraComponent = mIn["CameraComponent"];
		auto& cameraProps = cameraComponent["Camera"];
		SceneCamera camera;

		camera.setProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());

		camera.setPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
		camera.setPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
		camera.setPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

		camera.setOrthographicSize(cameraProps["OrthographicSize"].as<float>());
		camera.setOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
		camera.setOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());

		return CameraComponent(camera, cameraComponent["Primary"].as<bool>(), cameraComponent["FixedAspectRatio"].as<bool>());
	}

	template<>
	Ref<SpriteRendererComponent> YAMLParser::decodeObject<SpriteRendererComponent>(Entity entity, YAML::Node node)
	{
		auto spriteRendererComponent = node["SpriteRendererComponent"];
		if (spriteRendererComponent)
		{
			auto& src = entity.addComponent<SpriteRendererComponent>();
			src.colour = spriteRendererComponent["Colour"].as<glm::vec4>();
			return CreateRef< SpriteRendererComponent>(src);
		}
		return nullptr;
	}

	template<>
	SpriteRendererComponent YAMLParser::decodeObject<SpriteRendererComponent>()
	{
		LAB_CORE_ASSERT(mIn["SpriteRendererComponent"], "File must contain a SpriteRenderer component!");
		auto& spriteRendererComponent = mIn["SpriteRendererComponent"];

		return SpriteRendererComponent(spriteRendererComponent["Colour"].as<glm::vec4>());
	}

	template<>
	Ref<Entity> YAMLParser::decodeObject<Entity, Ref<Scene>>(Ref<Scene> scene, YAML::Node entity)
	{
		uint64_t uuid = entity["Entity"].as<uint64_t>(); // TODO

		std::string name;
		auto tagComponent = entity["TagComponent"];
		if (tagComponent)
			name = tagComponent["Tag"].as<std::string>();

		LAB_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);

		Entity deserializedEntity = scene->CreateEntity(name);

		//Must add new components here as they are added.
		if (!decodeObject<TransformComponent>(deserializedEntity, entity)) return nullptr;
		if (!decodeObject<CameraComponent>(deserializedEntity, entity)) return nullptr;
		if (!decodeObject<SpriteRendererComponent>(deserializedEntity, entity)) return nullptr;

		return CreateRef<Entity>(deserializedEntity);
	}

	template<>
	Ref<Entity> YAMLParser::decodeObject<Entity>(Ref<Scene> scene)
	{
		if (!mIn["Entity"]) return false;

		auto entity = mIn["Entity"];
		return decodeObject<Entity>(scene, entity);
	}

	template<>
	Ref<Scene> YAMLParser::decodeObject<Scene>(Ref<Scene> scene)
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
				Ref<Entity> deserialisedEntity = decodeObject<Entity>(scene, entity);
				if (!deserialisedEntity) return nullptr;
			}
		}

		return scene;
	}
}