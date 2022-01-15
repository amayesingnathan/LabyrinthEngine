#pragma once

#include "Labyrinth/Maths/Vector2D.h"
#include "Labyrinth/Maths/Quad.h"

#include "SceneCamera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

struct SDL_Texture;
struct SDL_Rect;

namespace Labyrinth {

	struct Component
	{
		enum class Types { Null = -1, Tag = 0, Velocity, Transform, Sprite, Controller, Collider, Tile, Script };

		class Entity* entity = nullptr;
		Types derived = Types::Null;

		Component() = default;
		Component(class Entity* entt, Types type);
		virtual ~Component() {}
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
	

	};

	struct ColliderComponent : public Component
	{
		//THIS MUST MIRROR ENUM "Type" IN XMLParser.h
		enum class Type { Solid = 0, Trigger };

		Quad collider;
		Type type;
		void(*triggerFunc)();

		ColliderComponent& operator=(const ColliderComponent&) = default;

		ColliderComponent() = default;
		ColliderComponent(class Entity* entt, ColliderComponent::Type = Type::Solid, void (*fcnPtr)() = nullptr);
		ColliderComponent(class Entity* entt, const SDL_Rect& rect, ColliderComponent::Type = Type::Solid, void (*fcnPtr)() = nullptr);
	};


	struct KeyboardController : public Component
	{
		KeyboardController() = default;
		KeyboardController(class Entity* entt);
	};

	struct NativeScriptComponent
	{
		class ScriptableEntity* instance = nullptr;

		std::function<void()> instantiateScript;
		std::function<void()> destroyScript;

		std::vector<std::function<void()>> runScripts;

		template<typename T>
		void bind(std::vector<std::function<void()>> scripts)
		{
			LAB_CORE_ASSERT(std::is_base_of<ScriptableEntity, T>::value);
			instantiateScript = [&]() { instance = new T(); };
			destroyScript = [&]() { delete dynamic_cast<T*>(instance); instance = nullptr; };

			runScripts.reserve(scripts.size());
			
			for (auto script : scripts)
				runScripts.emplace_back(script);
		}
	};

	struct ScriptComponent
	{
		std::string filepath = "";
		void* input = nullptr;
		mutable void* output = nullptr;

		ScriptComponent() = default;
		ScriptComponent(const std::string& scriptPath, void* in, void* out)
			: filepath(scriptPath), input(in), output(out){}
		ScriptComponent(const ScriptComponent&) = default;
	};

	struct ScriptTriggerComponent
	{	
		struct TriggerSpec
		{
			Labyrinth::EventType type = EventType::None;
		};

		TriggerSpec trigger;
		const ScriptComponent& startScript;

		ScriptTriggerComponent(const TriggerSpec& triggerSpec, const ScriptComponent& script)
			: trigger(triggerSpec), startScript(script) {}
		ScriptTriggerComponent(const ScriptTriggerComponent&) = default;
	};

#if 0
	struct SpriteComponent : public Component
	{

		SDL_Texture* texture;
		SDL_Rect srcRect, destRect;

		bool animated;
		int frames;
		int speed;

		Animation currAnimation;
		std::map<std::string, Animation> animations;

		SDL_RendererFlip spriteFlip;

		SpriteComponent(class Entity* entt, const char* path, const SDL_Rect& src, bool mAnimated = false);
		SpriteComponent(class Entity* entt, SDL_Texture& tex, const SDL_Rect& src, bool mAnimated = false);

		SpriteComponent& operator=(const SpriteComponent&) = default;

		void setTex(const char* path);

		void setSubTex(int x, int y, int w, int h);

		void addAnimation(std::string anim, int i, int f, int s)
		{
			animations.emplace(anim, Animation(i, f, s));
		}
	};

#endif

	struct SpriteRendererComponent
	{
		glm::vec4 colour{ 1.0f, 1.0f, 1.0f, 1.0f };

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& rgba)
			: colour(rgba) {}
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


	struct TileComponent : public Component
	{
		Vector2D position;
		struct SpriteComponent* sprite;

		TileComponent() = default;

		TileComponent(class Entity* entt, const SDL_Rect& src, const SDL_Rect& dest, SDL_Texture& tex);

		void remove();

	};


	struct TransformComponent
	{
		glm::vec3 translation = glm::vec3{ 0.0f };
		glm::vec3 rotation = glm::vec3{ 0.0f };
		glm::vec3 scale = glm::vec3{ 1.0f };

		glm::mat4 lastSafeTransform{ 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3 trans) : translation(trans) {}

		glm::mat4 getTransform() const
		{
			glm::mat4 rot = glm::toMat4(glm::quat(rotation));

			return glm::translate(glm::mat4(1.0f), translation)
				* rot
				* glm::scale(glm::mat4(1.0f), scale);
		}

		// These operators function the same as "getTransform()" for easier 
		// implicit direct access when using the component
		operator glm::mat4 () 
		{ 
			glm::mat4 rot = glm::rotate(glm::mat4(1.0f), rotation.x, { 1, 0, 0 })
				* glm::rotate(glm::mat4(1.0f), rotation.y, { 0, 1, 0 })
				* glm::rotate(glm::mat4(1.0f), rotation.z, { 0, 0, 1 });

			return glm::translate(glm::mat4(1.0f), translation)
				* rot
				* glm::scale(glm::mat4(1.0f), scale);
		}

		operator const glm::mat4 () const 
		{
			glm::mat4 rot = glm::rotate(glm::mat4(1.0f), rotation.x, { 1, 0, 0 })
				* glm::rotate(glm::mat4(1.0f), rotation.y, { 0, 1, 0 })
				* glm::rotate(glm::mat4(1.0f), rotation.z, { 0, 0, 1 });

			return glm::translate(glm::mat4(1.0f), translation)
				* rot
				* glm::scale(glm::mat4(1.0f), scale);
		}


	};


	struct VelocityComponent : public Component
	{
		Vector2D vel;

		VelocityComponent& operator=(const VelocityComponent&) = default;

		template<typename T>
		VelocityComponent(class Entity* entt, T velocity) :
			Component(entt, Types::Velocity), vel(velocity) {}
	};

}