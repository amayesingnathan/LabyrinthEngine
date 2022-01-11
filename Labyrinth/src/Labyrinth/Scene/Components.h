#pragma once

#include "Labyrinth/Maths/Vector2D.h"
#include "Labyrinth/Maths/Quad.h"

#include "ScriptableEntity.h"

#include "SceneCamera.h"

#include <glm/glm.hpp>

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
		ScriptableEntity* instance = nullptr;

		ScriptableEntity* (*instantiateScript)() = nullptr;
		void (*destroyScript)(NativeScriptComponent*) = nullptr;

		template<typename T>
		void bind()
		{
			instantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			destroyScript = [](NativeScriptComponent* sc) { delete sc->instance; sc->instance = nullptr; };
		}
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
		glm::mat4 transform{ 1.0f };
		glm::mat4 lastSafeTransform{ 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::mat4 trans) : transform(trans) {}

		operator glm::mat4& () { return transform; }
		operator const glm::mat4& () const { return transform; }
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