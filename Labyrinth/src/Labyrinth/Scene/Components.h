#pragma once

#include "Labyrinth/Maths/Vector2D.h"

#include "Labyrinth/Core/UUID.h"
#include "Labyrinth/Scene/SceneCamera.h"
#include "Labyrinth/Scripting/ScriptObject.h"
#include "Labyrinth/Renderer/SubTexture.h"
#include "Labyrinth/Tilemap/Tilemap.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <variant>

namespace Labyrinth {

	//////////////////////////////////////////
	//    General object data components	//
	//////////////////////////////////////////

	struct IDComponent
	{
		UUID id;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;

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
		CameraComponent(const CameraComponent&) = default;
		CameraComponent(const SceneCamera & cam, bool prime = true, bool fixedAR = false)
			: camera(cam), primary(prime), fixedAspectRatio(fixedAR) {}
	
		operator SceneCamera& () { return camera; }

	};

	struct SpriteRendererComponent
	{
		enum class TexType
		{
			None, Texture, SubTexture
		};

		u8 layer = 0;
		static const u8 MaxLayers = std::numeric_limits<u8>::max();

		TexType type = TexType::None;
		glm::vec4 colour = { 1.0f, 1.0f, 1.0f, 1.0f };
		AssetHandle handle = 0;
		f32 tilingFactor = 1.0f;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(u8 layer)
			: layer(layer), colour({ 1.0f, 1.0f, 1.0f, 1.0f }) {}
		SpriteRendererComponent(const glm::vec4& rgba, u8 layer = 0)
			: layer(layer), colour(rgba) {}

		bool hasTex() const 
		{ 
			return handle != 0;
		}

		// Get normalised layer value
		f32 getNLayer() const { return (Cast<f32>(layer) / Cast<f32>(MaxLayers)); }
	};

	struct CircleRendererComponent
	{
		u8 layer = 0;
		static const u8 MaxLayers = std::numeric_limits<u8>::max();

		glm::vec4 colour{ 1.0f, 1.0f, 1.0f, 1.0f };
		f32 thickness = 1.0f;

		CircleRendererComponent() = default;
		CircleRendererComponent(u8 layer)
			: layer(layer), colour({ 1.0f, 1.0f, 1.0f, 1.0f }) {}

		// Get normalised layer value
		f32 getNLayer() const { return (Cast<f32>(layer) / Cast<f32>(MaxLayers)); }
	};

	struct TagComponent
	{
		std::string tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string & tag) : tag(tag) {}

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
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& trans, const glm::vec3& rot, const glm::vec3& sc) 
			: translation(trans), rotation(rot), scale(sc) {}

		glm::mat4 getTransform() const
		{
			glm::mat4 rot = glm::toMat4(glm::quat(rotation));

			return glm::translate(glm::mat4(1.0f), translation)
				* rot
				* glm::scale(glm::mat4(1.0f), scale);
		}

		// These operators function the same as "getTransform()" for easier 
		// implicit direct access when using the component
		operator glm::mat4 () const
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
		ChildControllerComponent(const ChildControllerComponent&) = default;

		bool isZero() const 
		{ 
			return (deltaTranslation == Zero::Vec3 &&
				deltaRotation== Zero::Vec3 &&
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
		Ref<ScriptObject> instance = nullptr;
		bool initialised = false;

		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent&) = default;
		ScriptComponent(const std::string& name) : className(name) {}
	};

	class NativeScript;

	struct NativeScriptComponent
	{
		NativeScript* instance = nullptr;
		std::function<NativeScript* ()> instantiateScript;
		std::function<void()> destroyScript;

		bool complete = false;

		template<typename T>
		void bind()
		{
			instantiateScript = []() { return static_cast<NativeScript*>(new T()); };
			destroyScript = [this]() { delete this->instance; this->instance = nullptr; };
		}

		NativeScriptComponent() = default;
		NativeScriptComponent(const NativeScriptComponent& other)
			: instantiateScript(other.instantiateScript), destroyScript(other.destroyScript) {}
	};


	// Physics

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
		RigidBodyComponent(const RigidBodyComponent&) = default;
	};

	struct BoxColliderComponent
	{
		glm::vec2 halfExtents = { 0.5f, 0.5f };
		glm::vec2 offset = { 0.0f, 0.0f };
		f32 friction = 1.0f;
		f32 density = 0.5f;
		f32 restitution = 0.0f;
		f32 restitutionThreshold = 0.5f;

		void* runtimeFixture = nullptr;

		BoxColliderComponent() = default;
		BoxColliderComponent(const BoxColliderComponent&) = default;
	};

	struct CircleColliderComponent
	{
		f32 radius = 0.5f;
		glm::vec2 offset = { 0.0f, 0.0f };
		f32 friction = 1.0f;
		f32 density = 0.5f;
		f32 restitution = 0.0f;
		f32 restitutionThreshold = 0.5f;

		void* runtimeFixture = nullptr;

		CircleColliderComponent() = default;
		CircleColliderComponent(const CircleColliderComponent&) = default;
	};

	// Tilemaps

	struct TilemapControllerComponent
	{
		AssetHandle tilemapHandle = 0;
		std::unordered_map<TilePos, UUID> tileBehaviour;
	};

	template<typename... Component>
	struct ComponentGroup {};
	using AllComponents = ComponentGroup
		<
		NodeComponent,
		TransformComponent,
		SpriteRendererComponent,
		CircleRendererComponent,
		CameraComponent,
		RigidBodyComponent,
		BoxColliderComponent,
		CircleColliderComponent,
		NativeScriptComponent,
		ScriptComponent,
		TilemapControllerComponent
	>;
}