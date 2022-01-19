#include "Lpch.h"
#include "YAML.h"

#include "Labyrinth/Scene/Scene.h"
#include "Labyrinth/Scene/Entity.h"
#include "Labyrinth/Scene/Components.h"

#if 0

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

#endif

	namespace Labyrinth {

		template<typename T>
		void YAMLParser::encodeObject(const T& data)
		{
#ifndef LAB_PLATFORM_LINUX
		static_assert(false);
#endif
		}

		template<typename T>
		T YAMLParser::decodeObject()
		{
#ifndef LAB_PLATFORM_LINUX
		static_assert(false);
#endif
		}

		template<typename T, typename Target>
		Ref<T> YAMLParser::decodeObject(Target data)
		{
#ifndef LAB_PLATFORM_LINUX
		static_assert(false);
#endif
		}

		template<typename T, typename Target>
		Ref<T> YAMLParser::decodeObject(Target data, const YAML::Node& node)
		{
#ifndef LAB_PLATFORM_LINUX
		static_assert(false);
#endif
		}

	}

#if 0

	template<typename T>
	void YAMLParser::serialiseObject(T& data)
	{
		static_assert(false);
	}

	template<>
	void YAMLParser::serialiseObject<Entity>(Entity& entity)
	{
		mOut << YAML::BeginMap; // Entity
		mOut << YAML::Key << "Entity" << YAML::Value << entity.getID();

		if (entity.hasComponent<TagComponent>())
		{
			serialiseComponent<TagComponent>(entity);
		}

		if (entity.hasComponent<TransformComponent>())
		{
			serialiseComponent<TransformComponent>(entity);
		}

		if (entity.hasComponent<CameraComponent>())
		{
			serialiseComponent<CameraComponent>(entity);
		}

		if (entity.hasComponent<SpriteRendererComponent>())
		{
			serialiseComponent<SpriteRendererComponent>(entity);
		}

		mOut << YAML::EndMap; // Entity
	}

	//template<>
	//void YAMLParser::serialiseObject<Scene>(Scene& scene)
	//{
	//	mOut << YAML::BeginMap;
	//	mOut << YAML::Key << "Scene" << YAML::Value << "Untitled";
	//	mOut << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

	//	scene.mRegistry.each([&](auto entityID)
	//	{
	//		Entity entity = { entityID, &scene };
	//		if (!entity)
	//			return;

	//		serialiseObject<Entity>(entity);
	//	});
	//	
	//	mOut << YAML::EndSeq;
	//	mOut << YAML::EndMap;
	//}

	template<typename T>
	void YAMLParser::serialiseComponent(T& data)
	{
		static_assert(false);
	}

	template<>
	void YAMLParser::serialiseComponent<TagComponent>(TagComponent& tag)
	{
		mOut << YAML::Key << "TagComponent";
		mOut << YAML::BeginMap; // TagComponent

		mOut << YAML::Key << "Tag" << YAML::Value << tag.tag;

		mOut << YAML::EndMap; // TagComponent
	}

	template<>
	void YAMLParser::serialiseComponent<TransformComponent>(TransformComponent& transform)
	{
		mOut << YAML::Key << "TransformComponent";
		mOut << YAML::BeginMap; // TransformComponent

		mOut << YAML::Key << "Translation" << YAML::Value << transform.translation;
		mOut << YAML::Key << "Rotation" << YAML::Value << transform.rotation;
		mOut << YAML::Key << "Scale" << YAML::Value << transform.scale;

		mOut << YAML::EndMap; // TransformComponent
	}

	template<>
	void YAMLParser::serialiseComponent<CameraComponent>(CameraComponent& cameraComponent)
	{
		mOut << YAML::Key << "CameraComponent";
		mOut << YAML::BeginMap; // CameraComponent

		auto& camera = cameraComponent.camera;

		mOut << YAML::Key << "Camera" << YAML::Value;
		mOut << YAML::BeginMap; // Camera
		mOut << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.getProjectionType();
		mOut << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.getPerspectiveVerticalFOV();
		mOut << YAML::Key << "PerspectiveNear" << YAML::Value << camera.getPerspectiveNearClip();
		mOut << YAML::Key << "PerspectiveFar" << YAML::Value << camera.getPerspectiveFarClip();
		mOut << YAML::Key << "OrthographicSize" << YAML::Value << camera.getOrthographicSize();
		mOut << YAML::Key << "OrthographicNear" << YAML::Value << camera.getOrthographicNearClip();
		mOut << YAML::Key << "OrthographicFar" << YAML::Value << camera.getOrthographicFarClip();
		mOut << YAML::EndMap; // Camera

		mOut << YAML::Key << "Primary" << YAML::Value << cameraComponent.primary;
		mOut << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.fixedAspectRatio;

		mOut << YAML::EndMap; // CameraComponent
	}

	template<>
	void YAMLParser::serialiseComponent<SpriteRendererComponent>(SpriteRendererComponent& srComponent)
	{
		mOut << YAML::Key << "SpriteRendererComponent";
		mOut << YAML::BeginMap; // SpriteRendererComponent

		mOut << YAML::Key << "Colour" << YAML::Value << srComponent.colour;

		mOut << YAML::EndMap; // SpriteRendererComponent
	}




	bool YAMLParser::deserialiseScene(Ref<Scene> scene)
	{
		if (!mIn["Scene"]) return false;

		bool success = true;

		std::string sceneName = mIn["Scene"].as<std::string>();
		LAB_CORE_TRACE("Deserializing scene '{0}'", sceneName);

		auto entities = mIn["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				success = deserialiseEntity(scene, entity);
				if (!success) return success;
			}
		}

		return true;
	}

	template<typename T>
	T* YAMLParser::deserialiseComponent(Entity& entity, YAML::Node node)
	{
		static_assert(false);
	}

	//Must add new components here as they are added.
	template<>
	TransformComponent* YAMLParser::deserialiseComponent<TransformComponent>(Entity& entity, YAML::Node node)
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
	CameraComponent* YAMLParser::deserialiseComponent<CameraComponent>(Entity& entity, YAML::Node node)
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
	SpriteRendererComponent* YAMLParser::deserialiseComponent<SpriteRendererComponent>(Entity& entity, YAML::Node node)
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

	bool YAMLParser::deserialiseEntity(Ref<Scene> scene, YAML::Node entity)
	{
		uint64_t uuid = entity["Entity"].as<uint64_t>(); // TODO

		std::string name;
		auto tagComponent = entity["TagComponent"];
		if (tagComponent)
			name = tagComponent["Tag"].as<std::string>();

		LAB_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);

		Entity deserializedEntity = scene->CreateEntity(name);

		//Must add new components here as they are added.
		deserialiseComponent<TransformComponent>(deserializedEntity, entity);
		deserialiseComponent<CameraComponent>(deserializedEntity, entity);
		deserialiseComponent<SpriteRendererComponent>(deserializedEntity, entity);

		return true;
	}

	bool YAMLParser::deserialiseEntity(Ref<Scene> scene)
	{
		if (!mIn["Entity"]) return false;

		auto entity = mIn["Entity"];
		deserialiseEntity(scene, entity);

		return true;
	}
}

#endif