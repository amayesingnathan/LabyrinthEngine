#include "Lpch.h"
#include "SceneSerialiser.h"

#include "Entity.h"
#include "Components.h"

#include "Labyrinth/Assets/AssetManager.h"
#include "Labyrinth/IO/YAML.h"

#include <fstream>

namespace Labyrinth {

	SceneSerialiser::SceneSerialiser(const Ref<Scene>& scene)
		: mScene(scene)
	{
	}

	void SceneSerialiser::serialise(const std::filesystem::path& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene";
		out << YAML::Value << mScene->getName();

		out << YAML::Key << "Entities";
		out << YAML::Value << YAML::BeginSeq;

		// Sort entities by UUID (for better serializing)
		std::map<UUID, entt::entity> sortedEntityMap;
		mScene->getEntitiesWith<IDComponent>().each([&](auto entity, auto& idComp) 
		{
			sortedEntityMap[idComp.id] = entity;
		});

		// Serialize sorted entities
		for (auto [id, entity] : sortedEntityMap)
			SerializeEntity(out, { entity, mScene });

		out << YAML::EndSeq;

		out << YAML::EndMap;

		FileUtils::Write(filepath, out.c_str());
	}

	bool SceneSerialiser::deserialise(const std::filesystem::path& filepath)
	{
		std::string str;
		FileUtils::Read(filepath, str);

		YAML::Node data = YAML::Load(str);
		if (!data["Scene"])
			return false;

		std::string sceneName = data["Scene"].as<std::string>();
		LAB_CORE_INFO("Deserializing scene '{0}'", sceneName);

		if (sceneName == "Untitled")
		{
			std::filesystem::path path = filepath;
			sceneName = path.stem().string();
		}

		mScene->setName(sceneName);

		auto entities = data["Entities"];
		if (entities)
			DeserializeEntities(entities, mScene);

		return true;
	}

	void SceneSerialiser::SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		UUID uuid = entity.getComponent<IDComponent>().id;
		out << YAML::BeginMap; // Entity
		out << YAML::Key << "Entity";
		out << YAML::Value << uuid;

		if (entity.hasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap; // TagComponent

			const auto& tag = entity.getComponent<TagComponent>().tag;
			out << YAML::Key << "Tag" << YAML::Value << tag;

			out << YAML::EndMap; // TagComponent
		}


		if (entity.hasComponent<NodeComponent>())
		{
			auto& nodeComponent = entity.getComponent<NodeComponent>();
			out << YAML::Key << "Parent" << YAML::Value << nodeComponent.parent;

			out << YAML::Key << "Children";
			out << YAML::Value << YAML::BeginSeq;

			for (UUID child : nodeComponent.children)
			{
				out << YAML::BeginMap;
				LAB_SERIALISE_PROPERTY(Handle, child, out);
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
		}

		if (entity.hasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap; // TransformComponent

			auto& transform = entity.getComponent<TransformComponent>();
			LAB_SERIALISE_PROPERTY(Position, transform.translation, out);
			LAB_SERIALISE_PROPERTY(Rotation, transform.rotation, out);
			LAB_SERIALISE_PROPERTY(Scale, transform.scale, out);

			out << YAML::EndMap; // TransformComponent
		}

		if (entity.hasComponent<SpriteRendererComponent>())
		{
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap; // SpriteRendererComponent

			auto& sprite = entity.getComponent<SpriteRendererComponent>();
			LAB_SERIALISE_PROPERTY(Type, (i32)sprite.type, out);
			LAB_SERIALISE_PROPERTY(Layer, (i32)sprite.layer, out);
			LAB_SERIALISE_PROPERTY(Handle, sprite.handle, out);
			LAB_SERIALISE_PROPERTY(Colour, sprite.colour, out);
			LAB_SERIALISE_PROPERTY(TilingFactor, sprite.tilingFactor, out);

			out << YAML::EndMap; // SpriteRendererComponent
		}

		if (entity.hasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap; // CameraComponent

			auto& cameraComponent = entity.getComponent<CameraComponent>();

			out << YAML::Key << "Camera";
			out << YAML::BeginMap; // Camera
			LAB_SERIALISE_PROPERTY(ProjectionType, (i32)cameraComponent.camera.getProjectionType(), out);
			LAB_SERIALISE_PROPERTY(PerspectiveFOV, cameraComponent.camera.getPerspectiveVerticalFOV(), out);
			LAB_SERIALISE_PROPERTY(PerspectiveNear, cameraComponent.camera.getPerspectiveNearClip(), out);
			LAB_SERIALISE_PROPERTY(PerspectiveFar, cameraComponent.camera.getPerspectiveFarClip(), out);
			LAB_SERIALISE_PROPERTY(OrthographicSize, cameraComponent.camera.getOrthographicSize(), out);
			LAB_SERIALISE_PROPERTY(OrthographicNear, cameraComponent.camera.getOrthographicNearClip(), out);
			LAB_SERIALISE_PROPERTY(OrthographicFar, cameraComponent.camera.getOrthographicFarClip(), out);
			out << YAML::EndMap; // Camera

			LAB_SERIALISE_PROPERTY(Primary, cameraComponent.primary, out);
			LAB_SERIALISE_PROPERTY(FixedAspectRatio, cameraComponent.fixedAspectRatio, out);

			out << YAML::EndMap; // CameraComponent
		}

		if (entity.hasComponent<RigidBodyComponent>())
		{
			out << YAML::Key << "RigidBodyComponent";
			out << YAML::BeginMap; // RigidBodyComponent

			auto& rbComponent = entity.getComponent<RigidBodyComponent>();
			LAB_SERIALISE_PROPERTY(Type, (i32)rbComponent.type, out);
			LAB_SERIALISE_PROPERTY(FixedRotation, rbComponent.fixedRotation, out);

			out << YAML::EndMap; // RigidBodyComponent
		}

		if (entity.hasComponent<BoxColliderComponent>())
		{
			out << YAML::Key << "BoxColliderComponent";
			out << YAML::BeginMap; // BoxColliderComponent

			auto& bcComponent = entity.getComponent<BoxColliderComponent>();
			LAB_SERIALISE_PROPERTY(HalfExtents, bcComponent.halfExtents, out);
			LAB_SERIALISE_PROPERTY(Offset, bcComponent.offset, out);
			LAB_SERIALISE_PROPERTY(Density, bcComponent.density, out);
			LAB_SERIALISE_PROPERTY(Friction, bcComponent.friction, out);
			LAB_SERIALISE_PROPERTY(Restitution, bcComponent.restitution, out);
			LAB_SERIALISE_PROPERTY(RestitutionThreshold, bcComponent.restitutionThreshold, out);

			out << YAML::EndMap; // BoxColliderComponent
		}

		if (entity.hasComponent<CircleColliderComponent>())
		{
			out << YAML::Key << "CircleColliderComponent";
			out << YAML::BeginMap; // CircleColliderComponent

			auto& ccComponent = entity.getComponent<CircleColliderComponent>();
			LAB_SERIALISE_PROPERTY(Radius, ccComponent.radius, out);
			LAB_SERIALISE_PROPERTY(Offset, ccComponent.offset, out);
			LAB_SERIALISE_PROPERTY(Density, ccComponent.density, out);
			LAB_SERIALISE_PROPERTY(Friction, ccComponent.friction, out);
			LAB_SERIALISE_PROPERTY(Restitution, ccComponent.restitution, out);
			LAB_SERIALISE_PROPERTY(RestitutionThreshold, ccComponent.restitutionThreshold, out);

			out << YAML::EndMap; // CircleColliderComponent
		}

		if (entity.hasComponent<ChildControllerComponent>())
		{
			out << YAML::Key << "ChildControllerComponent";
			out << YAML::BeginMap; // ChildControllerComponent

			LAB_SERIALISE_PROPERTY(Controller, true, out);

			out << YAML::EndMap; // ChildControllerComponent
		}

		if (entity.hasComponent<ScriptComponent>())
		{
			out << YAML::Key << "ScriptComponent";
			out << YAML::BeginMap; // ScriptComponent

			auto& scriptComponent = entity.getComponent<ScriptComponent>();
			LAB_SERIALISE_PROPERTY(ClassName, scriptComponent.className, out);

			out << YAML::EndMap; // ScriptComponent
		}

		if (entity.hasComponent<TilemapControllerComponent>())
		{
			out << YAML::Key << "TilemapControllerComponent";
			out << YAML::BeginMap; // TilemapControllerComponent

			auto& tilemapController = entity.getComponent<TilemapControllerComponent>();
			LAB_SERIALISE_PROPERTY(Tilemap, tilemapController.tilemapHandle, out);

			out << YAML::Key << "Behaviour";
			out << YAML::Value << YAML::BeginSeq;

			for (const auto& [pos, id] : tilemapController.tileBehaviour)
			{
				out << YAML::BeginMap;
				LAB_SERIALISE_PROPERTY(Position, pos, out);
				LAB_SERIALISE_PROPERTY(Entity, id, out);
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;

			out << YAML::EndMap; // TilemapControllerComponent
		}

		out << YAML::EndMap; // Entity
	}

	void SceneSerialiser::DeserializeEntities(YAML::Node& entitiesNode, Ref<Scene> scene)
	{
		for (auto entity : entitiesNode)
		{
			u64 uuid = entity["Entity"].as<u64>();

			std::string name;
			auto tagComponent = entity["TagComponent"];
			if (tagComponent)
				name = tagComponent["Tag"].as<std::string>();

			LAB_CORE_INFO("Deserialized Entity '{0}' with ID '{1}'", name, uuid);

			Entity deserializedEntity = scene->CreateEntityWithID(uuid, name);

			auto& nodeComponent = deserializedEntity.getComponent<NodeComponent>();
			u64 parentHandle = entity["Parent"] ? entity["Parent"].as<u64>() : 0;
			nodeComponent.parent = parentHandle;
			if (parentHandle)
				deserializedEntity.removeComponent<RootComponent>();

			auto children = entity["Children"];
			if (children)
			{
				for (auto child : children)
				{
					u64 childHandle = child["Handle"].as<u64>();
					nodeComponent.children.push_back(childHandle);
				}
			}

			auto transformComponent = entity["TransformComponent"];
			if (transformComponent)
			{
				// Entities always have transforms
				auto& transform = deserializedEntity.getComponent<TransformComponent>();
				LAB_DESERIALISE_PROPERTY_DEF(Position, transform.translation, transformComponent, glm::vec3{ 1.f });
				LAB_DESERIALISE_PROPERTY_DEF(Rotation, transform.rotation, transformComponent, glm::vec3{ 1.f });
				LAB_DESERIALISE_PROPERTY_DEF(Scale, transform.scale, transformComponent, glm::vec3{ 1.f });
			}
			else
				deserializedEntity.removeComponent<TransformComponent>();

			auto cameraComponent = entity["CameraComponent"];
			if (cameraComponent)
			{
				auto cameraProps = cameraComponent["Camera"];

				auto& cc = deserializedEntity.addComponent<CameraComponent>();
				cc.camera.setProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<i32>());

				cc.camera.setPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<f32>());
				cc.camera.setPerspectiveNearClip(cameraProps["PerspectiveNear"].as<f32>());
				cc.camera.setPerspectiveFarClip(cameraProps["PerspectiveFar"].as<f32>());

				cc.camera.setOrthographicSize(cameraProps["OrthographicSize"].as<f32>());
				cc.camera.setOrthographicNearClip(cameraProps["OrthographicNear"].as<f32>());
				cc.camera.setOrthographicFarClip(cameraProps["OrthographicFar"].as<f32>());

				cc.primary = cameraComponent["Primary"].as<bool>();
				cc.fixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
			}

			auto spriteRendererComponent = entity["SpriteRendererComponent"];
			if (spriteRendererComponent)
			{
				auto& src = deserializedEntity.addComponent<SpriteRendererComponent>();

				src.type = (SpriteRendererComponent::TexType)spriteRendererComponent["Type"].as<i32>();
				src.layer = spriteRendererComponent["Layer"].as<u8>();
				deserializedEntity.getComponent<TransformComponent>().translation.z = src.getNLayer();
				src.handle = spriteRendererComponent["Handle"].as<u64>();
				src.colour = spriteRendererComponent["Colour"].as<glm::vec4>();
			}

			auto circleRendererComponent = entity["CircleRendererComponent"];
			if (circleRendererComponent)
			{
				auto& src = deserializedEntity.addComponent<CircleRendererComponent>();

				src.layer = circleRendererComponent["Layer"].as<u8>();
				deserializedEntity.getComponent<TransformComponent>().translation.z = src.getNLayer();

				src.colour = circleRendererComponent["Colour"].as<glm::vec4>();
				src.thickness = circleRendererComponent["Thickness"].as<f32>();
			}

			auto rigidBodyComponent = entity["RigidBodyComponent"];
			if (rigidBodyComponent)
			{
				auto& rbc = deserializedEntity.addComponent<RigidBodyComponent>();
				rbc.type = (RigidBodyComponent::BodyType)rigidBodyComponent["Type"].as<i32>();
				rbc.fixedRotation = rigidBodyComponent["FixedRotation"].as<bool>();
			}

			auto boxColliderComponent = entity["BoxColliderComponent"];
			if (boxColliderComponent)
			{

				auto& bcc = deserializedEntity.addComponent<BoxColliderComponent>();
				bcc.halfExtents = boxColliderComponent["HalfExtents"].as<glm::vec2>();
				bcc.offset = boxColliderComponent["Offset"].as<glm::vec2>();
				bcc.density = boxColliderComponent["Density"].as<f32>();
				bcc.friction = boxColliderComponent["Friction"].as<f32>();
				bcc.restitution = boxColliderComponent["Restitution"].as<f32>();
				bcc.restitutionThreshold = boxColliderComponent["RestitutionThreshold"].as<f32>();
			}

			auto circleColliderComponent = entity["CircleColliderComponent"];
			if (circleColliderComponent)
			{
				auto& ccc = deserializedEntity.addComponent<CircleColliderComponent>();
				ccc.radius = circleColliderComponent["Radius"].as<f32>();
				ccc.offset = circleColliderComponent["Offset"].as<glm::vec2>();
				ccc.density = circleColliderComponent["Density"].as<f32>();
				ccc.friction = circleColliderComponent["Friction"].as<f32>();
				ccc.restitution = circleColliderComponent["Restitution"].as<f32>();
				ccc.restitutionThreshold = circleColliderComponent["RestitutionThreshold"].as<f32>();
			}

			auto childControllerComponent = entity["ChildControllerComponent"];
			if (childControllerComponent)
				auto& ccc = deserializedEntity.addComponent<ChildControllerComponent>();

			auto scriptComponent = entity["ScriptComponent"];
			if (scriptComponent)
			{
				auto& sc = deserializedEntity.addComponent<ScriptComponent>();
				sc.className = scriptComponent["ClassName"].as<std::string>();
			}

			auto tmcComponent = entity["TilemapControllerComponent"];
			if (scriptComponent)
			{
				auto& tmcc = deserializedEntity.addComponent<TilemapControllerComponent>();

				u64 handle;
				LAB_DESERIALISE_PROPERTY(Tilemap, handle, scriptComponent);
				tmcc.tilemapHandle = handle;

				auto behaviour = scriptComponent["Behaviour"];
				if (behaviour)
				{
					for (auto entry : behaviour)
					{
						TilePos pos;
						u64 id;
						LAB_DESERIALISE_PROPERTY(Position, pos, entry);
						LAB_DESERIALISE_PROPERTY(Entity, id, entry);

						tmcc.tileBehaviour[pos] = id;
					}
				}
			}
		}
	}
}