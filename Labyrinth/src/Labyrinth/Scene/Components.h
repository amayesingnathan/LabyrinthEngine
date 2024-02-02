#pragma once

#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <Labyrinth/Assets/Asset.h>
#include <Labyrinth/Scene/SceneCamera.h>
#include <Labyrinth/Scripting/ScriptObject.h>
#include <Labyrinth/Tilemap/TileBehaviourLayer.h>

class b2World;
struct b2Vec2;

namespace Laby {

	//////////////////////////////////////////
	//    General object data components	//
	//////////////////////////////////////////

	struct IDComponent
	{
		UUID id;

		IDComponent() = default;

		operator UUID() const { return id; }
	};

	//Root components just indicates an entity has no parent.
	struct RootComponent
	{
		// Seem to need some actual data to be able to use as template parameter in addComponent
		// so just added a random zero byte.
		u8 data = 0x0;
		RootComponent() = default;
	};

	//Node component for use in parent/child relations
	struct NodeComponent
	{
		UUID parent = 0;
		std::vector<UUID> children = {};

		NodeComponent() = default;

		operator bool() { return parent; }
	};

	struct CameraComponent
	{
		SceneCamera camera;
		bool primary = true; //ToDo: Maybe move to Scene
		bool fixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const SceneCamera& cam, bool prime = true, bool fixedAR = false)
			: camera(cam), primary(prime), fixedAspectRatio(fixedAR) {}

		operator SceneCamera& () { return camera; }

	};

	struct SpriteRendererComponent
	{
		u8 layer = 0;

		RenderType type = RenderType::None;
		glm::vec4 colour = { 1.0f, 1.0f, 1.0f, 1.0f };
		AssetHandle handle = 0;
		f32 tilingFactor = 1.0f;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(u8 layer)
			: layer(layer), colour({ 1.0f, 1.0f, 1.0f, 1.0f }) {}
		SpriteRendererComponent(const glm::vec4& rgba, u8 layer = 0)
			: layer(layer), colour(rgba) {}
	};

	struct CircleRendererComponent
	{
		u8 layer = 0;

		glm::vec4 colour{ 1.0f, 1.0f, 1.0f, 1.0f };
		f32 thickness = 1.0f;

		CircleRendererComponent() = default;
		CircleRendererComponent(u8 layer)
			: layer(layer), colour({ 1.0f, 1.0f, 1.0f, 1.0f }) {}
	};

	struct TagComponent
	{
		std::string tag;

		TagComponent() = default;
		TagComponent(const std::string& tag) : tag(tag) {}

		operator std::string& () { return tag; }
		operator const std::string& () const { return tag; }

		operator const char* () { return tag.c_str(); }
	};

	struct TransformComponent
	{
		glm::vec3 translation = Zero::Vec3;
		glm::vec3 rotation = Zero::Vec3;
		glm::vec3 scale = glm::vec3{ 1.0f };

		TransformComponent() = default;
		TransformComponent(const glm::vec3& trans, const glm::vec3& rot, const glm::vec3& sc)
			: translation(trans), rotation(rot), scale(sc) {}

		glm::mat4 GetTransform() const
		{
			glm::mat4 rot = glm::toMat4(glm::quat(rotation));

			return glm::translate(glm::mat4(1.0f), translation)
				* rot
				* glm::scale(glm::mat4(1.0f), scale);
		}

		// These operators function the same as "getTransform()" for easier 
		// implicit direct access when using the component
		operator glm::mat4() const
		{
			glm::mat4 rot = glm::toMat4(glm::quat(rotation));

			return glm::translate(glm::mat4(1.0f), translation)
				* rot
				* glm::scale(glm::mat4(1.0f), scale);
		}
	};


	// Editor

	struct ChildControllerComponent
	{
		glm::vec3 deltaTranslation = Zero::Vec3;
		glm::vec3 deltaRotation = Zero::Vec3;
		glm::vec3 deltaScale = Zero::Vec3;

		ChildControllerComponent() = default;

		bool isZero() const
		{
			return (deltaTranslation == Zero::Vec3 &&
				deltaRotation == Zero::Vec3 &&
				deltaScale == Zero::Vec3);
		}

		void zero()
		{
			deltaTranslation = Zero::Vec3;
			deltaRotation = Zero::Vec3;
			deltaScale = Zero::Vec3;
		}
	};


	// Scripting

	struct ScriptComponent
	{
		std::string className;
		Ref<ScriptObject> instance;
		bool initialised = false;

		ScriptComponent() = default;
		ScriptComponent(std::string_view name) : className(name) {}
	};


	// Physics

	class ContactListener;

	struct Box2DWorldComponent
	{
		b2World* world = nullptr;
		ContactListener* contactListener = nullptr;

		Box2DWorldComponent() = default;
	};

	struct RigidBodyComponent
	{
		enum class BodyType { None = -1, Static = 0, Dynamic, Kinematic };
		BodyType type = BodyType::Static;
		bool fixedRotation = false;
		float mass = 1.0f;
		float linearDrag = 0.01f;
		float angularDrag = 0.05f;
		float gravityScale = 1.0f;

		void* runtimeBody = nullptr;

		RigidBodyComponent() = default;
	};

	struct BoxColliderComponent
	{
		glm::vec2 halfExtents = { 0.5f, 0.5f };
		glm::vec2 offset = { 0.0f, 0.0f };

		bool sensor = false;

		f32 friction = 0.25f;
		f32 density = 0.5f;
		f32 restitution = 0.0f;
		f32 restitutionThreshold = 0.5f;

		void* runtimeFixture = nullptr;

		BoxColliderComponent() = default;
	};

	struct CircleColliderComponent
	{
		f32 radius = 0.5f;
		glm::vec2 offset = { 0.0f, 0.0f };

		bool sensor = false;

		f32 friction = 0.25f;
		f32 density = 0.5f;
		f32 restitution = 0.0f;
		f32 restitutionThreshold = 0.5f;

		void* runtimeFixture = nullptr;

		CircleColliderComponent() = default;
	};

	struct ChainColliderComponent
	{
		glm::vec2* vertices = nullptr;
		i32 vertexCount = 0;

		bool sensor = false;

		f32 friction = 0.25f;
		f32 density = 0.5f;
		f32 restitution = 0.0f;
		f32 restitutionThreshold = 0.5f;

		void* runtimeFixture = nullptr;

		ChainColliderComponent() = default;
		ChainColliderComponent(const ChainColliderComponent&) = default;
		ChainColliderComponent(const ChainShape& shape) :
			vertices(shape.vertices), vertexCount(shape.vertexCount) {}
		~ChainColliderComponent() { delete[] vertices; }

		const b2Vec2* GetVertices() const { return (b2Vec2*)vertices; }
	};


	// Tilemaps

	struct TilemapComponent
	{
		AssetHandle mapHandle = 0;

		TilemapComponent() = default;
	};


	// Amimation

	struct AnimationComponent
	{
		AssetHandle handle = 0;

		u32 frameCounter = 0;
		u32 frameIndex = 0;

		bool playing = false;
		bool playOnce = false;

		AnimationComponent() = default;
	};

	using AllComponents = TypeList
	<
		NodeComponent,
		TransformComponent,
		SpriteRendererComponent,
		CircleRendererComponent,
		CameraComponent,
		RigidBodyComponent,
		BoxColliderComponent,
		CircleColliderComponent,
		ChainColliderComponent,
		ScriptComponent,
		TilemapComponent,
		AnimationComponent
	>;

	template<typename T>
	concept IsComponent = AllComponents::Contains<T>;

	template<IsComponent... Component>
	static void CopyComponent(TypeList<Component...>, const Registry& src, Registry& dest, const std::unordered_map<UUID, EntityID>& entMap)
	{
		([&]()
		{
			src.view<Component, IDComponent>().each([&dest, &entMap](auto entity, const auto& component, const auto& id)
			{
				LAB_CORE_ASSERT(entMap.count(id) != 0);
				dest.emplace_or_replace<Component>(entMap.at(id), component);
			});
		}(), ...);
	}
	static void CopyAllComponents(Registry& src, Registry& dest, const std::unordered_map<UUID, EntityID>& entMap)
	{
		CopyComponent(AllComponents{}, src, dest, entMap);
	}

	template<IsComponent... Component>
	static void CopyComponent(TypeList<Component...>, Entity src, Entity dest)
	{
		([&]()
		{
			if (src.HasComponent<Component>())
			dest.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
		}(), ...);
	}
	static void CopyAllComponents(Entity src, Entity dest)
	{
		CopyComponent(AllComponents{}, src, dest);
	}
}