#pragma once

#include "Labyrinth/Maths/Vector2D.h"

#include "Labyrinth/Assets/AssetGroup.h"
#include "Labyrinth/Core/UUID.h"
#include "Labyrinth/Scene/SceneCamera.h"
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
		enum class TexType { None = -1, Texture, SubTexture };
		struct NoTex {};
		using TextureComponent = std::variant<NoTex, Ref<Texture2D>, Ref<SubTexture2D>>;

		TexType type = TexType::None;

		uint8_t layer = 0;
		static const uint8_t MaxLayers = std::numeric_limits<uint8_t>::max();

		glm::vec4 colour{ 1.0f, 1.0f, 1.0f, 1.0f };
		TextureComponent texture = NoTex();
		float tilingFactor = 1.0f;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(uint8_t layer)
			: type(TexType::None), layer(layer), colour({ 1.0f, 1.0f, 1.0f, 1.0f }) {}
		SpriteRendererComponent(const glm::vec4& rgba, uint8_t layer = 0)
			: type(TexType::None), layer(layer), colour(rgba) {}
		SpriteRendererComponent(Ref<Texture2D> tex, float tf, uint8_t layer = 0)
			: type(TexType::Texture), layer(layer), texture(tex), tilingFactor(tf) {}
		SpriteRendererComponent(Ref<SubTexture2D> subtex, float tf, uint8_t layer = 0)
			: type(TexType::SubTexture), layer(layer), texture(subtex), tilingFactor(tf) {}

		bool hasTex() const 
		{ 
			return type != TexType::None; 
		}

		template<typename TexType>
		Ref<TexType> getTex() const
		{
			return std::get<Ref<TexType>>(texture);
		}

		// Get normalised layer value
		float getNLayer() const { return (Cast<float>(layer) / Cast<float>(MaxLayers)); }
	};

	struct CircleRendererComponent
	{
		uint8_t layer = 0;
		static const uint8_t MaxLayers = std::numeric_limits<uint8_t>::max();

		glm::vec4 colour{ 1.0f, 1.0f, 1.0f, 1.0f };
		float thickness = 1.0f;

		CircleRendererComponent() = default;
		CircleRendererComponent(uint8_t layer)
			: layer(layer), colour({ 1.0f, 1.0f, 1.0f, 1.0f }) {}

		// Get normalised layer value
		float getNLayer() const { return (Cast<float>(layer) / Cast<float>(MaxLayers)); }
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
	};


	// Physics

	struct RigidBodyComponent
	{
		enum class BodyType { Static = 0, Dynamic, Kinematic };
		BodyType type = BodyType::Static;
		bool fixedRotation = false;

		void* runtimeBody = nullptr;

		RigidBodyComponent() = default;
		RigidBodyComponent(const RigidBodyComponent&) = default;
	};

	struct BoxColliderComponent
	{
		glm::vec2 halfExtents = { 0.5f, 0.5f };
		glm::vec2 offset = { 0.0f, 0.0f };
		float friction = 1.0f;
		float density = 0.5f;
		float restitution = 0.0f;
		float restitutionThreshold = 0.5f;

		void* runtimeFixture = nullptr;

		BoxColliderComponent() = default;
		BoxColliderComponent(const BoxColliderComponent&) = default;
	};

	struct CircleColliderComponent
	{
		float radius = 0.5f;
		glm::vec2 offset = { 0.0f, 0.0f };
		float friction = 1.0f;
		float density = 0.5f;
		float restitution = 0.0f;
		float restitutionThreshold = 0.5f;

		void* runtimeFixture = nullptr;

		CircleColliderComponent() = default;
		CircleColliderComponent(const CircleColliderComponent&) = default;
	};


	// Tilemaps

	struct TilemapComponent
	{
		Ref<Tilemap> tilemap = nullptr;
		uint8_t layer = 0;

		TilemapComponent() = default;
		TilemapComponent(const std::string& name, uint8_t _layer = 0) : tilemap(Tilemap::Create(name)), layer(_layer) {}
		TilemapComponent(const TilemapComponent&) = default;

		const Ref<Texture2D>& getTex() const { return tilemap->getTex(); }
	};

	template<typename... Component>
	struct ComponentGroup {};
	using AllComponents = ComponentGroup
		<
		TransformComponent,
		SpriteRendererComponent,
		CircleRendererComponent,
		CameraComponent,
		RigidBodyComponent,
		BoxColliderComponent,
		CircleColliderComponent,
		TilemapComponent
	>;
}