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
		glm::vec3 translation = glm::vec3{ 0.0f };
		glm::vec3 rotation = glm::vec3{ 0.0f };
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

	struct ScriptComponent
	{
		std::string className;
		Ref<ScriptObject> instance = nullptr;

		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent&) = default;
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

	struct TilemapComponent
	{
		Ref<Tilemap> tilemap = nullptr;
		u8 layer = 0;

		TilemapComponent() = default;
		TilemapComponent(const std::string& name, u8 _layer = 0) : tilemap(Tilemap::Create(name)), layer(_layer) {}
		TilemapComponent(const TilemapComponent&) = default;

		Ref<Texture2D> getTex() const { return tilemap->getTex(); }
	};

	struct NodeComponent;

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
		TilemapComponent,
		NativeScriptComponent,
		ScriptComponent
	>;
}