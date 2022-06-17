#include "Lpch.h"
#include "Serialiser.h"

#include "Entity.h"
#include "Components.h"

#include <fstream>


namespace YAML {

	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			//node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

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

	YAML::Emitter& operator<<(YAML::Emitter& mOut, const glm::vec2& v)
	{
		mOut << YAML::Flow;
		mOut << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return mOut;
	}

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
	void YAMLParser::EncodeObject<std::vector<Ref<Texture2DSheet>>>(const std::vector<Ref<Texture2DSheet>>& sheets, bool flag)
	{
		BeginSequence("SpriteSheets");

		for (Ref<Texture2DSheet> sheet : sheets)
		{
			BeginObject();

			ObjectProperty("Name", sheet->getName());
			ObjectProperty("Source", sheet->getTex()->getPath());
			ObjectProperty("TileSize", sheet->getTileSize());

			EndObject();
		}

		EndSequence();
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
			BeginObject();
			ObjectProperty("EntID", Cast<uint32_t>(child));
			EndObject();
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

		ObjectProperty("Type", Cast<int>(srComponent.type));
		ObjectProperty("Layer", Cast<int>(srComponent.layer)); //Cast to int so it is encoded as number not char

		ObjectProperty("Colour", srComponent.colour);

		switch (srComponent.type)
		{
		case SpriteRendererComponent::TexType::Texture:
		{
			BeginObject("Texture");
			ObjectProperty("Source", srComponent.texture.tex->getPath());
			EndObject();
			break;
		}
		case SpriteRendererComponent::TexType::Tile:
		{
			BeginObject("Texture");

			ObjectProperty("Sheet", srComponent.texture.subtex->getSheet()->getName());
			ObjectProperty("SubTexName", srComponent.texture.subtex->getName());

			BeginSequence("Coordinates");
			for (size_t i = 0; i < 4; i++)
			{
				BeginObject();
				ObjectProperty(std::to_string(i), srComponent.texture.subtex->getTexCoords()[i]);
				EndObject();
			}

			EndSequence();

			EndObject();
			break;
		}
		default:
			break;
		}

		EndObject();
	}

	template<>
	void YAMLParser::EncodeObject<RigidBodyComponent>(const RigidBodyComponent& rbComponent, bool flag)
	{
		BeginObject("RigidBodyComponent");

		ObjectProperty("Type", Cast<int>(rbComponent.type));
		ObjectProperty("FixedRotation", rbComponent.fixedRotation);

		EndObject();
	}

	template<>
	void YAMLParser::EncodeObject<BoxColliderComponent>(const BoxColliderComponent& bcComponent, bool flag)
	{
		BeginObject("BoxColliderComponent");

		ObjectProperty("HalfExtents", bcComponent.halfExtents);
		ObjectProperty("Offset", bcComponent.offset);
		ObjectProperty("Density", bcComponent.density);
		ObjectProperty("Friction", bcComponent.friction);
		ObjectProperty("Restitution", bcComponent.restitution);
		ObjectProperty("RestitutionThreshold", bcComponent.restitutionThreshold);

		EndObject();
	}

	template<>
	void YAMLParser::EncodeObject<CircleColliderComponent>(const CircleColliderComponent& bcComponent, bool flag)
	{
		BeginObject("CircleColliderComponent");

		ObjectProperty("Radius", bcComponent.radius);
		ObjectProperty("Offset", bcComponent.offset);
		ObjectProperty("Density", bcComponent.density);
		ObjectProperty("Friction", bcComponent.friction);
		ObjectProperty("Restitution", bcComponent.restitution);
		ObjectProperty("RestitutionThreshold", bcComponent.restitutionThreshold);

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

		// Components guranteed to be on entity
		EncodeObject(entity.getComponent<TagComponent>());
		EncodeObject(entity.getComponent<NodeComponent>());
		EncodeObject(entity.getComponent<TransformComponent>());

		if (entity.hasComponent<CameraComponent>())
			EncodeObject(entity.getComponent<CameraComponent>());

		if (entity.hasComponent<SpriteRendererComponent>())
			EncodeObject(entity.getComponent<SpriteRendererComponent>());

		if (entity.hasComponent<RigidBodyComponent>())
			EncodeObject(entity.getComponent<RigidBodyComponent>());

		if (entity.hasComponent<BoxColliderComponent>())
			EncodeObject(entity.getComponent<BoxColliderComponent>());

		if (entity.hasComponent<CircleColliderComponent>())
			EncodeObject(entity.getComponent<CircleColliderComponent>());

		EndObject();
	}

	template<>
	void YAMLParser::EncodeObject<Ref<Scene>>(const Ref<Scene>& scene, bool flag)
	{
		BeginObject();
		ObjectProperty("Scene", "Untitled");

		std::vector<Ref<Texture2DSheet>> sheets;
		scene->getSheetsInUse(sheets);
		if (!sheets.empty())
			EncodeObject(sheets);

		BeginSequence("Entities");

		scene->view<IDComponent>().each([&](auto entityID, const auto& rc)
		{
			Entity entity = { entityID, scene };
			EncodeObject(entity);
		});

		EndSequence();
		EndObject();
	}

	static std::vector<Ref<Texture2DSheet>> SpriteSheets;

	static Entity CurrentParent = Entity();

	template<>
	void YAMLParser::DecodeObject<std::vector<Ref<Texture2DSheet>>>(std::vector<Ref<Texture2DSheet>>& sheetsOut)
	{
		if (!mIn["SpriteSheets"]) return;

		auto sheetsNode = mIn["SpriteSheets"];
		if (sheetsNode)
		{
			size_t count = 0;
			for (auto sheet : sheetsNode)
				count++;

			if (sheetsOut.capacity() < count)
				sheetsOut.reserve(count);

			for (auto sheet : sheetsNode)
			{
				std::string name = sheet["Name"].as<std::string>();
				std::string path = sheet["Source"].as<std::string>();
				glm::vec2 tileSize = sheet["TileSize"].as<glm::vec2>();

				sheetsOut.emplace_back(Texture2DSheet::CreateFromPath(path, tileSize, name));
			}
		}
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

			if (CurrentParent)
				entity.setParent(CurrentParent, nc);

			auto childCount = nodeComponent["ChildCount"];
			if (childCount)
				if (childCount.as<size_t>() != 0)
					nc.children.reserve(childCount.as<size_t>());

			auto children = nodeComponent["Children"];
			if (children)
			{
				// Save and then set new currentParent entity for setting 
				// the parent of all created child entities.
				Entity holdParent = CurrentParent;
				CurrentParent = entity;

				for (auto child : children)
					entity.addChild({ Cast<entt::entity>(child["EntID"].as<uint32_t>()), entity.getScene() });

				// Once all children have had parent set, restore the currentParent entity
				// to this entity's parent.
				CurrentParent = holdParent;
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
	Ref<CameraComponent> YAMLParser::DecodeObject<CameraComponent>(Entity entity, YAML::Node node)
	{
		auto cameraComponent = node["CameraComponent"];
		if (cameraComponent)
		{
			auto cameraProps = cameraComponent["Camera"];
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
	Ref<SpriteRendererComponent> YAMLParser::DecodeObject<SpriteRendererComponent>(Entity entity, YAML::Node node)
	{
		auto spriteRendererComponent = node["SpriteRendererComponent"];
		if (spriteRendererComponent)
		{
			auto& src = entity.addComponent<SpriteRendererComponent>();

			src.type = (SpriteRendererComponent::TexType)spriteRendererComponent["Type"].as<int>();
			src.layer = spriteRendererComponent["Layer"].as<uint8_t>();
			entity.getComponent<TransformComponent>().translation.z = src.getNLayer();

			src.colour = spriteRendererComponent["Colour"].as<glm::vec4>();

			switch (src.type)
			{
			case SpriteRendererComponent::TexType::None: break;
			case SpriteRendererComponent::TexType::Texture: break;
			case SpriteRendererComponent::TexType::Tile:
			{
				auto texture = spriteRendererComponent["Texture"];
				std::string sheetName = texture["Sheet"].as<std::string>();

				auto it = std::find_if(SpriteSheets.begin(), SpriteSheets.end(), [&](Ref<Texture2DSheet> match) 
					{
						return match->getName() == sheetName;
					});

				if (it != SpriteSheets.end())
				{
					std::string subTexName = texture["SubTexName"].as<std::string>();

					glm::vec2 subtexCoords[4];
					auto coordsSeq = texture["Coordinates"];
					if (coordsSeq)
					{
						int i = 0;
						for (auto coord : coordsSeq)
						{
							if (i >= 4) break;

							subtexCoords[i] = coord[std::to_string(i)].as<glm::vec2>();
							i++;
						}
					}

					if (!(*it)->hasSubTex(subTexName))
						src.texture.subtex = (*it)->createSubTex(subTexName, subtexCoords);
					else
						src.texture.subtex = (*it)->getSubTex(subTexName);
				}

				break;
			}
			}

			return CreateRef<SpriteRendererComponent>(src);
		}

		return nullptr;
	}

	template<>
	Ref<RigidBodyComponent> YAMLParser::DecodeObject<RigidBodyComponent>(Entity entity, YAML::Node node)
	{
		auto rbComponent = node["RigidBodyComponent"];
		if (rbComponent)
		{
			auto& rbc = entity.addComponent<RigidBodyComponent>();
			rbc.type = (RigidBodyComponent::BodyType)rbComponent["Type"].as<int>();
			rbc.fixedRotation = rbComponent["FixedRotation"].as<bool>();
			return CreateRef<RigidBodyComponent>(rbc);
		}
		return nullptr;
	}

	template<>
	Ref<BoxColliderComponent> YAMLParser::DecodeObject<BoxColliderComponent>(Entity entity, YAML::Node node)
	{
		auto bcComponent = node["BoxColliderComponent"];
		if (bcComponent)
		{
			auto& rbc = entity.addComponent<BoxColliderComponent>();
			rbc.halfExtents = bcComponent["HalfExtents"].as<glm::vec2>();
			rbc.offset = bcComponent["Offset"].as<glm::vec2>();
			rbc.density = bcComponent["Density"].as<float>();
			rbc.friction = bcComponent["Friction"].as<float>();
			rbc.restitution = bcComponent["Restitution"].as<float>();
			rbc.restitutionThreshold = bcComponent["RestitutionThreshold"].as<float>();
			return CreateRef<BoxColliderComponent>(rbc);
		}
		return nullptr;
	}

	template<>
	Ref<CircleColliderComponent> YAMLParser::DecodeObject<CircleColliderComponent>(Entity entity, YAML::Node node)
	{
		auto ccComponent = node["CircleColliderComponent"];
		if (ccComponent)
		{
			auto& rbc = entity.addComponent<CircleColliderComponent>();
			rbc.radius = ccComponent["Radius"].as<float>();
			rbc.offset = ccComponent["Offset"].as<glm::vec2>();
			rbc.density = ccComponent["Density"].as<float>();
			rbc.friction = ccComponent["Friction"].as<float>();
			rbc.restitution = ccComponent["Restitution"].as<float>();
			rbc.restitutionThreshold = ccComponent["RestitutionThreshold"].as<float>();
			return CreateRef<CircleColliderComponent>(rbc);
		}
		return nullptr;
	}

	template<>
	Ref<Entity> YAMLParser::DecodeObject<Entity, Ref<Scene>>(Ref<Scene> scene, YAML::Node entity)
	{
		UUID uuid(entity["Entity"].as<uint64_t>()); 

		std::string name;
		auto tagComponent = entity["TagComponent"];
		if (tagComponent)
			name = tagComponent["Tag"].as<std::string>();

		LAB_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);

		Entity deserializedEntity = scene->CreateEntityWithID(uuid, name);

		//Must add new components here as they are added.
		if (!DecodeObject<NodeComponent>(deserializedEntity, entity)) return nullptr;
		if (!DecodeObject<TransformComponent>(deserializedEntity, entity)) return nullptr;
		DecodeObject<CameraComponent>(deserializedEntity, entity);
		DecodeObject<SpriteRendererComponent>(deserializedEntity, entity);
		DecodeObject<RigidBodyComponent>(deserializedEntity, entity);
		DecodeObject<BoxColliderComponent>(deserializedEntity, entity);
		DecodeObject<CircleColliderComponent>(deserializedEntity, entity);

		return CreateRef<Entity>(deserializedEntity);
	}

	template<>
	Ref<Entity> YAMLParser::DecodeObject<Entity>(Ref<Scene> scene)
	{
		if (!mIn["Entity"]) return nullptr;

		auto entity = mIn["Entity"];
		return DecodeObject<Entity>(scene, entity);
	}

	template<>
	Ref<Scene> YAMLParser::DecodeObject<Scene>(Ref<Scene> scene)
	{
		if (!mIn["Scene"]) return nullptr;

		// Load spritesheets
		SpriteSheets.clear();
		DecodeObject(SpriteSheets);

		// Initialise current parent entity to null entity as first entity
		// read should have no parent.
		CurrentParent = Entity();

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