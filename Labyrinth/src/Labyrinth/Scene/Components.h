#pragma once

#include "Labyrinth/Maths/Vector2D.h"
#include "Labyrinth/Maths/Quad.h"

#include "ScriptableEntity.h"

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


	struct ScriptComponent : public Component
	{
		ScriptableEntity* instance = nullptr;

		ScriptableEntity* (*InstantiateScript)() = nullptr;
		void (*DestroyScript)(ScriptComponent*) = nullptr;

		void (*OnCreateFunc)(ScriptableEntity*) = nullptr;
		void (*OnDestroyFunc)(ScriptableEntity*) = nullptr;
		void (*OnUpdateFunc)(ScriptableEntity*) = nullptr;

		ScriptComponent() = default;
		ScriptComponent& operator=(const ScriptComponent&) = default;

		template<typename T>
		void bind()
		{
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](ScriptComponent* sc) { delete sc->instance; sc->instance = nullptr; };

			OnCreateFunc = [](ScriptableEntity* inst) { dynamic_cast<T*>(inst)->OnCreate(); };
			OnDestroyFunc = [](ScriptableEntity* inst) { dynamic_cast<T*>(inst)->OnDestroy(); };
			OnUpdateFunc = [](ScriptableEntity* inst) { dynamic_cast<T*>(inst)->OnUpdate(); };
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