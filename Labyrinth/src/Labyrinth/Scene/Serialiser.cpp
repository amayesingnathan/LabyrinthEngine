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

	static Entity currentParent = Entity();

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

	//Forward declare entity encoding for use in Node component encoding
	template<>
	void YAMLParser::EncodeObject<Entity>(const Entity& entity, bool includeComps);

	template<>
	void YAMLParser::EncodeObject<NodeComponent>(const NodeComponent& node, bool flag)
	{
		BeginObject("NodeComponent");

		ObjectProperty("ChildCount", node.children.size());

		BeginSequence("Children");
		for (auto& child : node.children)
		{
			EncodeObject(child);
		}
		EndSequence();

		EndObject();
	}

	template<>
	void YAMLParser::EncodeObject<TagComponent>(const TagComponent& tag, bool flag)
	{
		BeginObject("TagComponent");
		ObjectProperty("Tag", tag.tag);
		EndObject();
	}

	template<>
	void YAMLParser::EncodeObject<TransformComponent>(const TransformComponent& transform, bool flag)
	{
		BeginObject("TransformComponent");

		ObjectProperty("Translation", transform.translation);
		ObjectProperty("Rotation", transform.rotation);
		ObjectProperty("Scale", transform.scale);

		EndObject();
	}

	template<>
	void YAMLParser::EncodeObject<CameraComponent>(const CameraComponent& cameraComponent, bool flag)
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
	void YAMLParser::EncodeObject<SpriteRendererComponent>(const SpriteRendererComponent& srComponent, bool flag)
	{
		BeginObject("SpriteRendererComponent");

		ObjectProperty("Colour", srComponent.colour);

		EndObject();
	}

	template<>
	void YAMLParser::EncodeObject<Entity>(const Entity& entity, bool includeComps)
	{
		LAB_CORE_ASSERT(entity.hasComponent<IDComponent>());

		BeginObject();
		ObjectProperty("Entity", entity.getUUID());

		if (!includeComps)
		{
			EndObject();
			return;
		}

		if (entity.hasComponent<NodeComponent>())
		{
			const NodeComponent& nc = entity.getComponent<NodeComponent>();
			EncodeObject(nc);
		}

		if (entity.hasComponent<TagComponent>())
		{
			const TagComponent& tc = entity.getComponent<TagComponent>();
			EncodeObject(tc);
		}

		if (entity.hasComponent<TransformComponent>())
		{
			const TransformComponent& tc = entity.getComponent<TransformComponent>();
			EncodeObject(tc);
		}

		if (entity.hasComponent<CameraComponent>())
		{
			const CameraComponent& cc = entity.getComponent<CameraComponent>();
			EncodeObject(cc);
		}

		if (entity.hasComponent<SpriteRendererComponent>())
		{
			const SpriteRendererComponent& src = entity.getComponent<SpriteRendererComponent>();
			EncodeObject(src);
		}

		EndObject();
	}

	template<>
	void YAMLParser::EncodeObject<Ref<Scene>>(const Ref<Scene>& scene, bool flag)
	{
		BeginObject();
		ObjectProperty("Scene", "Untitled");
		BeginSequence("Entities");

		scene->mRegistry.view<RootComponent>().each([&](auto entityID, auto& rc)
			{
				Entity entity = { entityID, scene };
				if (!entity)
					return;

				EncodeObject(entity);
			});

		EndSequence();
		EndObject();
	}

	template<>
	Ref<Entity> YAMLParser::DecodeObject<Entity, Ref<Scene>>(Ref<Scene> scene, YAML::Node entity);


	template<>
	Ref<NodeComponent> YAMLParser::DecodeObject<NodeComponent>(Entity entity, YAML::Node node)
	{
		auto nodeComponent = node["NodeComponent"];
		if (nodeComponent)
		{
			// Entities always have node components
			auto& nc = entity.getComponent<NodeComponent>();

			if (currentParent)
				entity.setParent(currentParent, nc);

			auto childCount = nodeComponent["ChildCount"];
			if (childCount)
				if (childCount.as<size_t>() != 0)
					nc.children.reserve(childCount.as<size_t>());

			auto children = nodeComponent["Children"];
			if (children)
			{
				// Save and then set new currentParent entity for setting parent of all 
				// child entities to this entity.
				Entity holdParent = currentParent;
				currentParent = entity;

				for (auto child : children)
				{
					Ref<Entity> childEnt = DecodeObject<Entity>(entity.getScene(), child);
				}

				// Once all children have had parent set, restore the currentParent entity
				// to this entity's parent.
				currentParent = holdParent;
			}

			return CreateRef<NodeComponent>(nc);
		}
		return nullptr;
	}

	template<>
	Ref<TransformComponent> YAMLParser::DecodeObject<TransformComponent>(Entity entity, YAML::Node node)
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
	TransformComponent YAMLParser::DecodeObject<TransformComponent>()
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
	Ref<CameraComponent> YAMLParser::DecodeObject<CameraComponent>(Entity entity, YAML::Node node)
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
	CameraComponent YAMLParser::DecodeObject<CameraComponent>()
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
	Ref<SpriteRendererComponent> YAMLParser::DecodeObject<SpriteRendererComponent>(Entity entity, YAML::Node node)
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
	SpriteRendererComponent YAMLParser::DecodeObject<SpriteRendererComponent>()
	{
		LAB_CORE_ASSERT(mIn["SpriteRendererComponent"], "File must contain a SpriteRenderer component!");
		auto& spriteRendererComponent = mIn["SpriteRendererComponent"];

		return SpriteRendererComponent(spriteRendererComponent["Colour"].as<glm::vec4>());
	}

	template<>
	Ref<Entity> YAMLParser::DecodeObject<Entity, Ref<Scene>>(Ref<Scene> scene, YAML::Node entity)
	{
		uint64_t uuid = entity["Entity"].as<uint64_t>(); 

		std::string name;
		auto tagComponent = entity["TagComponent"];
		if (tagComponent)
			name = tagComponent["Tag"].as<std::string>();

		LAB_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);

		//Entity deserializedEntity = scene->CreateEntityWithID(uuid, name);
		Entity deserializedEntity = scene->CreateEntity(name);

		//Must add new components here as they are added.
		if (!DecodeObject<NodeComponent>(deserializedEntity, entity)) return nullptr;
		if (!DecodeObject<TransformComponent>(deserializedEntity, entity)) return nullptr;
		DecodeObject<CameraComponent>(deserializedEntity, entity);
		DecodeObject<SpriteRendererComponent>(deserializedEntity, entity);

		return CreateRef<Entity>(deserializedEntity);
	}

	template<>
	Ref<Entity> YAMLParser::DecodeObject<Entity>(Ref<Scene> scene)
	{
		if (!mIn["Entity"]) return false;

		auto entity = mIn["Entity"];
		return DecodeObject<Entity>(scene, entity);
	}

	template<>
	Ref<Scene> YAMLParser::DecodeObject<Scene>(Ref<Scene> scene)
	{
		if (!mIn["Scene"]) return nullptr;

		// Initialise current parent entity to null entity as first entity
		// read should have no parent.
		currentParent = Entity();

		std::string sceneName = mIn["Scene"].as<std::string>();
		LAB_CORE_TRACE("Deserializing scene '{0}'", sceneName);

		auto entities = mIn["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				Ref<Entity> deserialisedEntity = DecodeObject<Entity>(scene, entity);
				if (!deserialisedEntity) return nullptr;
			}
		}

		return scene;
	}
}