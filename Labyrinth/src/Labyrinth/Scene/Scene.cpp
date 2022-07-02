#include "Lpch.h"
#include "Scene.h"

#include "Entity.h"
#include "Components.h"

#include "Labyrinth/Renderer/Renderer2D.h"

#include <glm/glm.hpp>

#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_circle_shape.h>

namespace Labyrinth {

	static b2BodyType BodyTypeToBox2D(RigidBodyComponent::BodyType bodyType)
	{
		switch (bodyType)
		{
			case RigidBodyComponent::BodyType::Static: return b2_staticBody;
			case RigidBodyComponent::BodyType::Dynamic: return b2_dynamicBody;
			case RigidBodyComponent::BodyType::Kinematic: return b2_kinematicBody;
		}

		LAB_CORE_ASSERT(false, "Unknown body type");
		return b2_staticBody;
	}

	template<typename... Component>
	static void CopyComponent(const entt::registry& src, entt::registry& dest, const std::unordered_map<UUID, entt::entity>& entMap)
	{
		([&]()
		{
			src.view<Component, IDComponent>().each([&dest, &entMap](const auto& component, const auto& id)
				{
					LAB_CORE_ASSERT(entMap.count(id) != 0);
					dest.emplace_or_replace<Component>(entMap.at(id), component);
				});
		}(), ...);
	}
	template<typename... Component>
	static void CopyComponent(ComponentGroup<Component...>, const entt::registry& src, entt::registry& dest, const std::unordered_map<UUID, entt::entity>& entMap)
	{
		CopyComponent<Component...>(src, dest, entMap);
	}
	static void CopyAllComponents(entt::registry& src, entt::registry& dest, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		CopyComponent(AllComponents{}, src, dest, enttMap);
	}

	template<typename... Component>
	static void CopyComponent(Entity src, Entity dest)
	{
		([&]()
		{
			if (src.hasComponent<Component>())
				dest.addOrReplaceComponent<Component>(src.getComponent<Component>());
		}(), ...);
	}
	template<typename... Component>
	static void CopyComponent(ComponentGroup<Component...>, Entity src, Entity dest)
	{
		CopyComponent<Component...>(src, dest);
	}

	static void CopyAllComponents(Entity src, Entity dest)
	{
		CopyComponent(AllComponents{}, src, dest);
	}

	Scene::Scene(const std::string& name)
		: mName(name), mRenderStack(CreateSingle<RenderStack>())
	{
	}

	Scene::~Scene()
	{
		delete mPhysicsWorld;
	}

	Ref<Scene> Scene::Clone()
	{
		Ref<Scene> newScene = CreateRef<Scene>(mName);
		newScene->mViewportWidth = mViewportWidth;
		newScene->mViewportHeight = mViewportHeight;

		std::unordered_map<UUID, entt::entity> entMap;

		mRegistry.view<IDComponent, TagComponent>().each([this, &newScene, &entMap](const auto entity, const auto& idComp, const auto& tagComp)
		{
			entMap[idComp] = newScene->CreateEntityWithID(idComp, tagComp);
		});

		// Parent/child relations require more care to clone as parent/children are stored using entt::entity so need to find corresponding entities in cloned scene.
		mRegistry.view<IDComponent, NodeComponent>().each([this, &newScene, &entMap](const auto entity, const auto& idComp, const auto& nodeComp)
		{
			if (nodeComp.parent) {
				const auto& parentID = nodeComp.parent.getComponent<IDComponent>();
				Entity newEnt = { entMap.at(idComp), newScene };
				Entity newParent = { entMap.at(parentID), newScene };
				newEnt.setParent(newParent);
			}
		});

		CopyAllComponents(mRegistry, newScene->mRegistry, entMap);

		return newScene;
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
		Entity newEnt(mRegistry.create(), CreateRef(this));

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

		CopyAllComponents(copy, newEnt);

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
		
		CopyAllComponents(copy, newEnt);

		return newEnt;
	}

	void Scene::DestroyEntity(Entity entity, bool linkChildren)
	{
		Entity& parent = entity.getParent();

		if (parent)  //Remove entity from parents list of children
			parent.removeChild(entity);

		DestroyEntityR(entity, parent, linkChildren);
	}

	void Scene::DestroyEntityR(Entity entity, Entity parent, bool linkChildren)
	{
		// Set the parent of all entity's children (will be null entity if no parent)
		auto& children = entity.getChildren();
		for (auto& child : children)
		{
			if (linkChildren)
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

	void Scene::OnPhysicsStart()
	{
		mPhysicsWorld = new b2World({ 0.0f, -9.81f });

		mRegistry.view<TransformComponent, RigidBodyComponent>().each([this](auto e, const auto& trComponent, auto& rbComponent)
			{
				Entity entity(e, CreateRef(this));

				b2BodyDef bodyDef;
				bodyDef.type = BodyTypeToBox2D(rbComponent.type);
				bodyDef.position.Set(trComponent.translation.x, trComponent.translation.y);
				bodyDef.angle = trComponent.rotation.z;
				bodyDef.fixedRotation = rbComponent.fixedRotation;
				b2Body* body = mPhysicsWorld->CreateBody(&bodyDef);

				rbComponent.runtimeBody = body;

				if (entity.hasComponent<BoxColliderComponent>())
				{
					auto& bcComponent = entity.getComponent<BoxColliderComponent>();

					b2PolygonShape boxShape;
					boxShape.SetAsBox(bcComponent.halfExtents.x * trComponent.scale.x, bcComponent.halfExtents.y * trComponent.scale.y);

					b2FixtureDef fixtureDef;
					fixtureDef.shape = &boxShape;
					fixtureDef.density = bcComponent.density;
					fixtureDef.friction = bcComponent.friction;
					fixtureDef.restitution = bcComponent.restitution;
					fixtureDef.restitutionThreshold = bcComponent.restitutionThreshold;
					body->CreateFixture(&fixtureDef);
				}
				if (entity.hasComponent<CircleColliderComponent>())
				{
					auto& ccComponent = entity.getComponent<CircleColliderComponent>();

					b2CircleShape circleShape;
					circleShape.m_radius = ccComponent.radius * trComponent.scale.x;

					b2FixtureDef fixtureDef;
					fixtureDef.shape = &circleShape;
					fixtureDef.density = ccComponent.density;
					fixtureDef.friction = ccComponent.friction;
					fixtureDef.restitution = ccComponent.restitution;
					fixtureDef.restitutionThreshold = ccComponent.restitutionThreshold;
					body->CreateFixture(&fixtureDef);
				}
			});
	}

	void Scene::OnPhysicsStop()
	{
		delete mPhysicsWorld;
		mPhysicsWorld = nullptr;
	}

	void Scene::DrawScene(EditorCamera& camera)
	{
		Renderer2D::BeginState(camera);
		mRenderStack->draw();
		Renderer2D::EndState();

		mRenderStack->clearQuads();
	}

	void Scene::DrawScene(Camera& camera, const glm::mat4& transform)
	{
		Renderer2D::BeginState(camera, transform);
		mRenderStack->draw();
		Renderer2D::EndState();

		mRenderStack->clearQuads();
	}

	void Scene::BuildScene()
	{
		mRegistry.group<SpriteRendererComponent>(entt::get<TransformComponent>).each([this](auto entity, const auto& srComponent, const auto& trComponent)
		{
			mRenderStack->addQuad(trComponent, srComponent, Cast<int>(entity));
		});
		mRegistry.group<CircleRendererComponent>(entt::get<TransformComponent>).each([this](auto entity, const auto& crComponent, const auto& trComponent)
		{
			mRenderStack->addCircle(trComponent, crComponent, Cast<int>(entity));
		});
	}

	void Scene::StepPhysics2D(Timestep ts)
	{
		// Physics
		if (!mPhysicsWorld) return;
		
		const int32_t velIters = 6;
		const int32_t poslIters = 2;
		mPhysicsWorld->Step(ts, velIters, poslIters);

		mRegistry.group<RigidBodyComponent>(entt::get<TransformComponent>).each([this](const auto& rbComponent, auto& trComponent)
		{
			b2Body* body = Cast<b2Body>(rbComponent.runtimeBody);
			const auto& pos = body->GetPosition();
			trComponent.translation.x = pos.x;
			trComponent.translation.y = pos.y;
			trComponent.rotation.z = body->GetAngle();
		});
		
	}

	Entity Scene::FindEntity(UUID findID)
	{
		auto IDs = mRegistry.view<IDComponent>();
		for (auto entity : IDs)
		{
			auto& idc = IDs.get<IDComponent>(entity);
			if (idc.id == findID)
				return { entity, CreateRef(this) };
		}
		return Entity();
	}

	void Scene::getSheetsInUse(std::vector<Ref<Texture2DSheet>>& sheets)
	{
		sheets.clear();
		mRegistry.view<SpriteRendererComponent>().each([&sheets](auto entity, auto& srComponent)
		{
			if (srComponent.type == SpriteRendererComponent::TexType::SubTexture)
			{
				Ref<SubTexture2D> subTex = srComponent.getTex<SubTexture2D>();
				if (std::find_if(sheets.begin(), sheets.end(),
					[&subTex](const Ref<Texture2DSheet>& match) {
						return subTex->getBaseTex()->getPath() == match->getBaseTex()->getPath();
					})
					== sheets.end())
					sheets.emplace_back(subTex->getSheet());
			}
		});
	}

	void Scene::onRuntimeStart()
	{
		OnPhysicsStart();
	}

	void Scene::onRuntimeStop()
	{
		OnPhysicsStop();
	}

	void Scene::onSimulationStart()
	{
		OnPhysicsStart();
	}

	void Scene::onSimulationStop()
	{
		OnPhysicsStop();
	}
	
	void Scene::onUpdateRuntime(Timestep ts)
	{
		Entity camera = getPrimaryCameraEntity();
		if (!camera) return;

		StepPhysics2D(ts);

		BuildScene();
		DrawScene(camera.getComponent<CameraComponent>(), camera.getComponent<TransformComponent>());
	}

	void Scene::onUpdateSimulation(Timestep ts, EditorCamera& camera)
	{
		StepPhysics2D(ts);

		BuildScene();
		DrawScene(camera);
	}

	void Scene::onUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		BuildScene();
		DrawScene(camera);
	}

	void Scene::onViewportResize(uint32_t width, uint32_t height)
	{
		mViewportWidth = width;
		mViewportHeight = height;

		// Resize our non-FixedAspectRatio cameras
		mRegistry.view<CameraComponent>().each([this](auto& cameraComponent){
			if (!cameraComponent.fixedAspectRatio)
				cameraComponent.camera.setViewportSize(mViewportWidth, mViewportHeight);
		});
	}

	Entity Scene::getPrimaryCameraEntity()
	{
		Entity primaryCam;
		mRegistry.view<CameraComponent>().each([this, &primaryCam](auto entity, const auto& cameraComponent){
			if (cameraComponent.primary)
				primaryCam = { entity, CreateRef(this)};
		});
		return primaryCam;
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
	void Scene::onComponentAdded<CircleRendererComponent>(Entity entity, CircleRendererComponent& component)
	{
		// Overwrite transform component z value using render layer value
		auto& trans = entity.getComponent<TransformComponent>().translation;
		trans.z = component.getNLayer();
	}

	template<>
	void Scene::onComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{
	}

	template<>
	void Scene::onComponentAdded<RigidBodyComponent>(Entity entity, RigidBodyComponent& component)
	{
	}

	template<>
	void Scene::onComponentAdded<BoxColliderComponent>(Entity entity, BoxColliderComponent& component)
	{
	}

	template<>
	void Scene::onComponentAdded<CircleColliderComponent>(Entity entity, CircleColliderComponent& component)
	{
	}

}
