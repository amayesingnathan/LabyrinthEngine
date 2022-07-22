#include "Lpch.h"
#include "Serialiser.h"

#include "Entity.h"
#include "Components.h"

#include "Labyrinth/Assets/AssetManager.h"

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

			rhs.x = node[0].as<f32>();
			rhs.y = node[1].as<f32>();
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

			rhs.x = node[0].as<f32>();
			rhs.y = node[1].as<f32>();
			rhs.z = node[2].as<f32>();
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

			rhs.x = node[0].as<f32>();
			rhs.y = node[1].as<f32>();
			rhs.z = node[2].as<f32>();
			rhs.w = node[3].as<f32>();
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
	void YAMLParser::EncodeObject<Tex2DSheetGroup>(const Tex2DSheetGroup& sheets, bool flag)
	{
		BeginSequence("SpriteSheets");

		for (const auto& [key, sheet] : sheets)
		{
			BeginObject();

			ObjectProperty("Name", sheet->getName());
			ObjectProperty("Source", sheet->getBaseTex()->getPath());
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
			EncodeObject(child);

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
		ObjectProperty("ProjectionType", (i32)camera.getProjectionType());
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

		ObjectProperty("Type", Cast<i32>(srComponent.type));
		ObjectProperty("Layer", Cast<i32>(srComponent.layer)); //Cast to i32 so it is encoded as number not i8

		ObjectProperty("Colour", srComponent.colour);

		switch (srComponent.type)
		{
		case SpriteRendererComponent::TexType::Texture:
		{
			BeginObject("Texture");
			ObjectProperty("Source", srComponent.getTex<Texture2D>()->getPath());
			EndObject();
			break;
		}
		case SpriteRendererComponent::TexType::SubTexture:
		{
			BeginObject("Texture");

			const Ref<SubTexture2D>& tex = srComponent.getTex<SubTexture2D>();
			ObjectProperty("Sheet", tex->getSheet()->getName());
			ObjectProperty("SubTexName", tex->getName());

			BeginSequence("Coordinates");
			for (usize i = 0; i < 4; i++)
			{
				BeginObject();
				ObjectProperty(std::to_string(i), tex->getTexCoords()[i]);
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
	void YAMLParser::EncodeObject<CircleRendererComponent>(const CircleRendererComponent& srComponent, bool flag)
	{
		BeginObject("SpriteRendererComponent");

		ObjectProperty("Layer", Cast<i32>(srComponent.layer)); //Cast to i32 so it is encoded as number not i8
		ObjectProperty("Colour", srComponent.colour);
		ObjectProperty("Thickness", srComponent.thickness);

		EndObject();
	}

	template<>
	void YAMLParser::EncodeObject<RigidBodyComponent>(const RigidBodyComponent& rbComponent, bool flag)
	{
		BeginObject("RigidBodyComponent");

		ObjectProperty("Type", Cast<i32>(rbComponent.type));
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
		ObjectProperty("Scene", scene->getName());

		AssetHandle handle = AssetManager::GetAssetHandleFromPath("SpriteSheets.lss");
		EncodeObject(*AssetManager::GetAsset<Tex2DSheetGroup>(handle));

		BeginSequence("Entities");

		scene->getEntitiesWith<RootComponent>().each([&](auto entityID, const auto& rc)
		{
			Entity entity = { entityID, scene };
			EncodeObject(entity);
		});

		EndSequence();
		EndObject();
	}

	static Entity CurrentParent = Entity();

	template<>
	bool YAMLParser::DecodeObject<Ref<Tex2DSheetGroup>>(Ref<Tex2DSheetGroup> sheetsOut)
	{
		if (!mIn["SpriteSheets"]) return false;

		auto sheetsNode = mIn["SpriteSheets"];
		if (!sheetsNode) return false;

		usize count = 0;
		for (auto sheet : sheetsNode)
			count++;

		if (sheetsOut->capacity() < count)
			sheetsOut->reserve(count);

		for (auto sheet : sheetsNode)
		{
			std::string name = sheet["Name"].as<std::string>();
			std::string path = sheet["Source"].as<std::string>();
			glm::vec2 tileSize = sheet["TileSize"].as<glm::vec2>();

			sheetsOut->addOrGet(name, Texture2DSheet::Create(path, tileSize, name));
		}

		return true;
	}

	template<>
	bool YAMLParser::DecodeObject<Entity, Ref<Scene>>(Ref<Scene> scene, YAML::Node entity);

	template<>
	bool YAMLParser::DecodeObject<NodeComponent>(Entity entity, YAML::Node node)
	{
		auto nodeComponent = node["NodeComponent"];
		if (!nodeComponent) return false;

		// Entities always have node components
		auto& nc = entity.getComponent<NodeComponent>();

		if (CurrentParent)
			entity.setParent(CurrentParent, nc);

		auto childCount = nodeComponent["ChildCount"];
		if (childCount)
			if (childCount.as<usize>() != 0)
				nc.children.reserve(childCount.as<usize>());

		auto children = nodeComponent["Children"];
		if (children)
		{
			// Save and then set new currentParent entity for setting 
			// the parent of all created child entities.
			Entity holdParent = CurrentParent;
			CurrentParent = entity;

			for (auto child : children)
				DecodeObject<Entity>(entity.getScene(), child);

			// Once all children have had parent set, restore the currentParent entity
			// to this entity's parent.
			CurrentParent = holdParent;
		}

		return true;
	}

	template<>
	bool YAMLParser::DecodeObject<TransformComponent>(Entity entity, YAML::Node node)
	{
		auto transformComponent = node["TransformComponent"];
		if (!transformComponent) return false;

		// Entities always have transforms
		auto& tc = entity.getComponent<TransformComponent>();
		tc.translation = transformComponent["Translation"].as<glm::vec3>();
		tc.rotation = transformComponent["Rotation"].as<glm::vec3>();
		tc.scale = transformComponent["Scale"].as<glm::vec3>();
		return true;
	}

	template<>
	bool YAMLParser::DecodeObject<CameraComponent>(Entity entity, YAML::Node node)
	{
		auto cameraComponent = node["CameraComponent"];
		if (!cameraComponent) return false;

		auto cameraProps = cameraComponent["Camera"];
		auto& cc = entity.addComponent<CameraComponent>();
		cc.camera.setProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<i32>());

		cc.camera.setPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<f32>());
		cc.camera.setPerspectiveNearClip(cameraProps["PerspectiveNear"].as<f32>());
		cc.camera.setPerspectiveFarClip(cameraProps["PerspectiveFar"].as<f32>());

		cc.camera.setOrthographicSize(cameraProps["OrthographicSize"].as<f32>());
		cc.camera.setOrthographicNearClip(cameraProps["OrthographicNear"].as<f32>());
		cc.camera.setOrthographicFarClip(cameraProps["OrthographicFar"].as<f32>());

		cc.primary = cameraComponent["Primary"].as<bool>();
		cc.fixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();

		return true;
	}

	template<>
	bool YAMLParser::DecodeObject<SpriteRendererComponent>(Entity entity, YAML::Node node)
	{
		auto spriteRendererComponent = node["SpriteRendererComponent"];
		if (!spriteRendererComponent) return false;

		auto& src = entity.addComponent<SpriteRendererComponent>();

		src.type = (SpriteRendererComponent::TexType)spriteRendererComponent["Type"].as<i32>();
		src.layer = spriteRendererComponent["Layer"].as<u8>();
		entity.getComponent<TransformComponent>().translation.z = src.getNLayer();

		src.colour = spriteRendererComponent["Colour"].as<glm::vec4>();

		switch (src.type)
		{
		case SpriteRendererComponent::TexType::None: break;
		case SpriteRendererComponent::TexType::Texture:
		{
			auto texture = spriteRendererComponent["Texture"];
			src.texture = Texture2D::Create(texture["Source"].as<std::string>());
		}
		break;
		case SpriteRendererComponent::TexType::SubTexture:
		{
			auto texture = spriteRendererComponent["Texture"];
			std::string sheetName = texture["Sheet"].as<std::string>();

			AssetHandle handle = AssetManager::GetAssetHandleFromPath("SpriteSheets.lss");
			auto spriteSheets = AssetManager::GetAsset<Tex2DSheetGroup>(handle);
			auto it = std::find_if(spriteSheets->begin(), spriteSheets->end(), [&](const std::pair<std::string, Ref<Texture2DSheet>>& match)
				{
					return match.second->getName() == sheetName;
				});

			if (it != spriteSheets->end())
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


				if (!it->second->hasSubTex(subTexName))
					src.texture = it->second->createSubTex(subTexName, subtexCoords);
				else
					src.texture = it->second->getSubTex(subTexName);
			}
		}
		break;
		}

		return true;
	}

	template<>
	bool YAMLParser::DecodeObject<CircleRendererComponent>(Entity entity, YAML::Node node)
	{
		auto circleRendererComponent = node["CircleRendererComponent"];
		if (!circleRendererComponent) return false;

		auto& src = entity.addComponent<CircleRendererComponent>();

		src.layer = circleRendererComponent["Layer"].as<u8>();
		entity.getComponent<TransformComponent>().translation.z = src.getNLayer();

		src.colour = circleRendererComponent["Colour"].as<glm::vec4>();
		src.thickness = circleRendererComponent["Thickness"].as<f32>();

		return true;
	}

	template<>
	bool YAMLParser::DecodeObject<RigidBodyComponent>(Entity entity, YAML::Node node)
	{
		auto rbComponent = node["RigidBodyComponent"];
		if (!rbComponent) return false;

		auto& rbc = entity.addComponent<RigidBodyComponent>();
		rbc.type = (RigidBodyComponent::BodyType)rbComponent["Type"].as<i32>();
		rbc.fixedRotation = rbComponent["FixedRotation"].as<bool>();

		return true;
	}

	template<>
	bool YAMLParser::DecodeObject<BoxColliderComponent>(Entity entity, YAML::Node node)
	{
		auto bcComponent = node["BoxColliderComponent"];
		if (!bcComponent) return false;

		auto& rbc = entity.addComponent<BoxColliderComponent>();
		rbc.halfExtents = bcComponent["HalfExtents"].as<glm::vec2>();
		rbc.offset = bcComponent["Offset"].as<glm::vec2>();
		rbc.density = bcComponent["Density"].as<f32>();
		rbc.friction = bcComponent["Friction"].as<f32>();
		rbc.restitution = bcComponent["Restitution"].as<f32>();
		rbc.restitutionThreshold = bcComponent["RestitutionThreshold"].as<f32>();

		return true;
	}

	template<>
	bool YAMLParser::DecodeObject<CircleColliderComponent>(Entity entity, YAML::Node node)
	{
		auto ccComponent = node["CircleColliderComponent"];
		if (!ccComponent) return false;

		auto& rbc = entity.addComponent<CircleColliderComponent>();
		rbc.radius = ccComponent["Radius"].as<f32>();
		rbc.offset = ccComponent["Offset"].as<glm::vec2>();
		rbc.density = ccComponent["Density"].as<f32>();
		rbc.friction = ccComponent["Friction"].as<f32>();
		rbc.restitution = ccComponent["Restitution"].as<f32>();
		rbc.restitutionThreshold = ccComponent["RestitutionThreshold"].as<f32>();

		return true;
	}

	template<>
	bool YAMLParser::DecodeObject<Entity, Ref<Scene>>(Ref<Scene> scene, YAML::Node entity)
	{
		UUID uuid(entity["Entity"].as<u64>());

		std::string name;
		auto tagComponent = entity["TagComponent"];
		if (tagComponent)
			name = tagComponent["Tag"].as<std::string>();

		LAB_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);

		Entity deserializedEntity = scene->CreateEntityWithID(uuid, name);

		//Must add new components here as they are added.
		DecodeObject<NodeComponent>(deserializedEntity, entity);
		DecodeObject<TransformComponent>(deserializedEntity, entity);
		DecodeObject<CameraComponent>(deserializedEntity, entity);
		DecodeObject<SpriteRendererComponent>(deserializedEntity, entity);
		DecodeObject<RigidBodyComponent>(deserializedEntity, entity);
		DecodeObject<BoxColliderComponent>(deserializedEntity, entity);
		DecodeObject<CircleColliderComponent>(deserializedEntity, entity);

		return true;
	}

	template<>
	bool YAMLParser::DecodeObject<Entity>(Ref<Scene> scene)
	{
		if (!mIn["Entity"]) return false;

		auto entity = mIn["Entity"];
		return DecodeObject<Entity>(scene, entity);
	}

	template<>
	bool YAMLParser::DecodeObject<Scene>(Ref<Scene> scene)
	{
		if (!mIn["Scene"]) return false;

		// Load spritesheets
		AssetHandle handle = AssetManager::GetAssetHandleFromPath("SpriteSheets.lss");
		DecodeObject<Ref<Tex2DSheetGroup>(AssetManager::GetAsset<Tex2DSheetGroup>(handle));

		// Initialise current parent entity to null entity as first entity
		// read should have no parent.
		CurrentParent = Entity();

		std::string sceneName = mIn["Scene"].as<std::string>();
		LAB_CORE_TRACE("Deserializing scene '{0}'", sceneName);
		scene->setName(sceneName);

		auto entities = mIn["Entities"];
		if (!entities) return false;

		for (auto entity : entities)
			if (!DecodeObject<Entity>(scene, entity)) return false;

		return true;
	}
}