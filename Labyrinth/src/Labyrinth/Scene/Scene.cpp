#include "Lpch.h"
#include "Scene.h"

#include "Entity.h"
#include "Components.h"

#include "Labyrinth/Renderer/Renderer2D.h"

#include <glm/glm.hpp>

namespace Labyrinth {

	static void DoMath(const glm::mat4& transform)
	{

	}

	static void OnTransformConstruct(entt::registry, Entity entity)
	{

	}

	Scene::Scene()
	{
	}

	Scene::~Scene()
	{
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity newEnt(mRegistry.create(), this);
		newEnt.addComponent<TransformComponent>();
		auto tag = newEnt.addComponent<TagComponent>();
		tag = name.empty() ? "Entity" : name;

		return newEnt;
	}

	void Scene::onUpdate(Timestep ts)
	{
		auto group = mRegistry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : group)
		{
			auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

			Renderer2D::DrawQuad(transform, sprite.colour);
		}
	}

}
