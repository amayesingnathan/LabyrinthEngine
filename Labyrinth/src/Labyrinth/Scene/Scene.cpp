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

	Entity Scene::CreateEntity(const std::string& name, Entity parent)
	{
		return CreateEntityWithID(UUID(), name, parent);
	}

	Entity Scene::CreateEntityWithID(const UUID& id, const std::string& name)
	{
		return CreateEntityWithID(id, name, Entity());
	}

	Entity Scene::CreateEntityWithID(const UUID& id, const std::string& name, Entity parent)
	{
		Entity newEnt(mRegistry.create(), CreateRefFromThis(this));

		newEnt.addComponent<IDComponent>(id);
		newEnt.addComponent<TransformComponent>();

		newEnt.addComponent<RootComponent>();
		auto& node = newEnt.addComponent<NodeComponent>();
		newEnt.setParent(parent, node);

		auto& tag = newEnt.addComponent<TagComponent>();
		tag = name.empty() ? "Entity" : name;

		return newEnt;
	}

	Entity Scene::CloneEntity(Entity copy)
	{
		auto& tag = copy.getComponent<TagComponent>();
		Entity newEnt = CreateEntity(tag.tag);

		auto nodeCopy = copy.getComponent<NodeComponent>();
		newEnt.setParent(nodeCopy.parent);

		// Use counting loop to prevent iterator invalidation
		size_t copyChildCount = nodeCopy.children.size();
		for (size_t i = 0; i < copyChildCount; i++)
			CloneChild(nodeCopy.children[i], newEnt);

		auto& trans = newEnt.getComponent<TransformComponent>();
		trans = copy.getComponent<TransformComponent>();

		if (copy.hasComponent<CameraComponent>())
			newEnt.addComponent<CameraComponent>(copy.getComponent<CameraComponent>());
		if (copy.hasComponent<SpriteRendererComponent>())
			newEnt.addComponent<SpriteRendererComponent>(copy.getComponent<SpriteRendererComponent>());

		return newEnt;
	}

	Entity Scene::CloneChild(Entity copy, Entity newParent)
	{
		auto& tag = copy.getComponent<TagComponent>();
		Entity newEnt = CreateEntity(tag.tag);

		auto nodeCopy = copy.getComponent<NodeComponent>();
		newEnt.setParent(newParent);

		// Use counting loop to prevent iterator invalidation
		size_t copyChildCount = nodeCopy.children.size();
		for (size_t i = 0; i < copyChildCount; i++)
			CloneChild(nodeCopy.children[i], newEnt);

		auto& trans = newEnt.getComponent<TransformComponent>();
		trans = copy.getComponent<TransformComponent>();

		if (copy.hasComponent<CameraComponent>())
			newEnt.addComponent<CameraComponent>(copy.getComponent<CameraComponent>());
		if (copy.hasComponent<SpriteRendererComponent>())
			newEnt.addComponent<SpriteRendererComponent>(copy.getComponent<SpriteRendererComponent>());

		return newEnt;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		auto& parent = entity.getParent();

		if (parent)  //Remove entity from parents list of children
			parent.removeChild(entity);

		DestroyEntityR(entity, parent);
	}

	void Scene::DestroyEntityR(Entity entity, Entity parent)
	{
		//Set this to true to link parent of entity with children of entity 
		//instead of destroying all child entities.
		static bool sLinkOnDestroy = false;

		// Set the parent of all entity's children (will be null entity if no parent)
		auto& children = entity.getChildren();
		for (auto& child : children)
		{
			if (sLinkOnDestroy)
			{
				if (parent)
					child.setParent(parent);
				else
					DestroyEntityR(child, entity);
			}
			else DestroyEntityR(child, entity);
		}

		mRegistry.destroy(entity);
	}

	Entity Scene::FindEntity(UUID findID)
	{
		auto IDs = mRegistry.view<IDComponent>();
		for (auto entity : IDs)
		{
			auto& idc = IDs.get<IDComponent>(entity);
			if (idc.id == findID)
				return { entity, CreateRefFromThis(this) };
		}
		return Entity();
	}

	void Scene::getSheetsInUse(std::vector<Ref<Texture2DSheet>>& sheets)
	{
		sheets.clear();
		mRegistry.view<SpriteRendererComponent>().each([&sheets](auto entity, auto& srComponent)
			{
				if (srComponent.type == SpriteRendererComponent::TexType::Tile)
				{
					if (std::find_if(sheets.begin(), sheets.end(),
						[&](Ref<Texture2DSheet> match) {
							return srComponent.texture.subtex->getTex()->getPath() == match->getTex()->getPath();
						})
						== sheets.end())
						sheets.emplace_back(srComponent.texture.subtex->getSheet());
				}
			});
	}
	
	void Scene::onUpdateRuntime(Timestep ts)
	{
		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		
		mRegistry.view<TransformComponent, CameraComponent>().each([this, &mainCamera, &cameraTransform](auto entity, auto& trComponent, auto& camComponent)
		{
			if (camComponent.primary)
			{
				mainCamera = &camComponent.camera;
				cameraTransform = trComponent;
			}
		});
		
		if (mainCamera)
		{
			RenderStack renderStack;

			mRegistry.view<TransformComponent, SpriteRendererComponent>().each([&renderStack](auto entity, auto& trComponent, auto& srComponent)
				{
					renderStack.addQuad(trComponent, srComponent, Cast<int>(entity));
				});

			Renderer2D::BeginState(*mainCamera, cameraTransform);
			renderStack.draw();
			Renderer2D::EndState();

			renderStack.clear();
		}

	}

	void Scene::onUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		RenderStack renderStack;

		mRegistry.view<TransformComponent, SpriteRendererComponent>().each([&renderStack](auto entity, const auto& trComponent, const auto& srComponent)
			{
				renderStack.addQuad(trComponent, srComponent, Cast<int>(entity));
			});

		Renderer2D::BeginState(camera);
		renderStack.draw();
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

#ifdef LAB_PLATFORM_WINDOWS
	template<typename T>
	void Scene::onComponentAdded(Entity entity, T& component)
	{
		static_assert(false);
	}
#endif

	template<>
	void Scene::onComponentAdded<RootComponent>(Entity entity, RootComponent& component)
	{
	}

	template<>
	void Scene::onComponentAdded<NodeComponent>(Entity entity, NodeComponent& component)
	{
	}

	template<>
	void Scene::onComponentAdded<IDComponent>(Entity entity, IDComponent& component)
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
		// Overwrite transform component z value using render layer value
		auto& trans = entity.getComponent<TransformComponent>().translation;
		trans.z = component.getNLayer();
	}

	template<>
	void Scene::onComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{
	}

}
