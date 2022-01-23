#include "Lpch.h"
#include "Scene.h"

#include "Entity.h"
#include "Components.h"

#include "Labyrinth/Renderer/Renderer2D.h"

#include <glm/glm.hpp>

namespace Labyrinth {

	Scene::Scene()
	{
	}

	Scene::~Scene()
	{
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateEntityWithID(UUID(), name, Entity());
	}

	Entity Scene::CreateEntity(const std::string& name, const Entity& parent)
	{
		return CreateEntityWithID(UUID(), name, parent);
	}

	Entity Scene::CreateEntityWithID(const UUID& id, const std::string& name)
	{
		return CreateEntityWithID(id, name, Entity());
	}

	Entity Scene::CreateEntityWithID(const UUID& id, const std::string& name, const Entity& parent)
	{
		Entity newEnt(mRegistry.create(), CreateRefFromThis(this));

		newEnt.addComponent<IDComponent>(id);
		newEnt.addComponent<TransformComponent>();

		auto& node = newEnt.addComponent<NodeComponent>();
		node.parent = parent;
		if (!parent)
			newEnt.addComponent<RootComponent>();

		auto& tag = newEnt.addComponent<TagComponent>();
		tag = name.empty() ? "Entity" : name;

		return newEnt;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		auto& parent = entity.getParent();

		if (parent)  //Remove entity from parents list of children
			parent.removeChild(entity);

		//Set this to true to link parent of entity with children of entity 
		//instead of destroying all child entities.
		static bool sLinkOnDestroy = false;

		//Set the parent of all entity's children (will be null entity if no parent)
		for (auto child : entity.getChildren())
			if (sLinkOnDestroy)
				child.setParent(parent);
			else
				DestroyEntity(child);

		mRegistry.destroy(entity);
	}

	void Scene::onUpdateRuntime(Timestep ts)
	{

		{	// Update Scripts
			mRegistry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc) {
				if (!nsc.instance)
				{
					nsc.instantiateScript();
				}

				nsc.instance->onNativeScript(nsc);

			});
		}

		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		{
			auto view = mRegistry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto& [trans, cam] = view.get<TransformComponent, CameraComponent>(entity);

				if (cam.primary)
				{
					mainCamera = &cam.camera;
					cameraTransform = trans;
				}
			}
		}
		
		if (mainCamera)
		{

			Renderer2D::BeginState(*mainCamera, cameraTransform);

			auto group = mRegistry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group)
			{
				auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

				Renderer2D::DrawSprite(transform, sprite, (int)entity);
			}

			Renderer2D::EndState();

		}

	}

	void Scene::onUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		Renderer2D::BeginState(camera);

		auto group = mRegistry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : group)
		{
			auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

			Renderer2D::DrawSprite(transform.getTransform(), sprite, (int)entity);
		}

		Renderer2D::EndState();
	}

	void Scene::onViewportResize(uint32_t width, uint32_t height)
	{
		mViewportWidth = width;
		mViewportHeight = height;

		// Resize our non-FixedAspectRatio cameras
		auto view = mRegistry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.fixedAspectRatio)
				cameraComponent.camera.setViewportSize(width, height);
		}
	}

	Entity Scene::getPrimaryCameraEntity()
	{
		auto view = mRegistry.view<CameraComponent>();
		for (auto entity : view)
		{
			const auto& camera = view.get<CameraComponent>(entity);
			if (camera.primary)
				return Entity{ entity,CreateRefFromThis(this) };
		}
		return {};
	}

	template<typename T>
	void Scene::onComponentAdded(Entity entity, T& component)
	{
		static_assert(false);
	}

	template<>
	void Scene::onComponentAdded<RootComponent>(Entity entity, RootComponent& component)
	{
	}

	template<>
	void Scene::onComponentAdded<NodeComponent>(Entity entity, NodeComponent& component)
	{
	}

	template<>
	void Scene::onComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{
	}

	template<>
	void Scene::onComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		if (mViewportWidth > 0 && mViewportHeight > 0)
			component.camera.setViewportSize(mViewportWidth, mViewportHeight);
	}

	template<>
	void Scene::onComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{
	}

	template<>
	void Scene::onComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{
	}

	template<>
	void Scene::onComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{
	}

}
