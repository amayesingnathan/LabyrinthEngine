#include "Lpch.h"
#include "Scene.h"

#include "Entity.h"
#include "Components.h"

#include <Labyrinth/Assets/AssetManager.h>
#include <Labyrinth/Renderer/Renderer2D.h>
#include <Labyrinth/Scripting/ScriptEngine.h>
#include <Labyrinth/Scripting/NativeScript.h>

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
		Ref<Scene> newScene = Ref<Scene>::Create(mName);
		newScene->mViewportWidth = mViewportWidth;
		newScene->mViewportHeight = mViewportHeight;

		std::unordered_map<UUID, entt::entity> entMap;

		mRegistry.view<IDComponent, TagComponent>().each([&](const auto& idComp, const auto& tagComp)
		{
			entMap[idComp] = newScene->CreateEntityWithID(idComp, tagComp);
		});

		CopyAllComponents(mRegistry, newScene->mRegistry, entMap);

		newScene->mRegistry.view<IDComponent, NodeComponent>().each([&](auto e, const auto& idComp, const auto& nodeComp)
		{
			if (nodeComp.parent)
			{
				Entity entity{ e, newScene };
				entity.removeComponent<RootComponent>();
			}
		});

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
		Entity newEnt(mRegistry.create(), Ref<Scene>(this));

		newEnt.addComponent<IDComponent>(id);
		newEnt.addComponent<TransformComponent>();

		newEnt.addComponent<RootComponent>();
		auto& node = newEnt.addComponent<NodeComponent>();
		newEnt.setParent(parent, node);

		auto& tag = newEnt.addComponent<TagComponent>();
		tag = name.empty() ? "Entity" : name;

		mEntityMap[id] = (entt::entity)newEnt.getEntID();

		return newEnt;
	}

	Entity Scene::CloneEntity(Entity copy)
	{
		auto& tag = copy.getComponent<TagComponent>();
		Entity newEnt = CreateEntity(tag.tag);

		auto nodeCopy = copy.getComponent<NodeComponent>();
		newEnt.setParent(findEntity(nodeCopy.parent));

		// Use counting loop to prevent iterator invalidation
		usize copyChildCount = nodeCopy.children.size();
		for (usize i = 0; i < copyChildCount; i++)
			CloneChild(findEntity(nodeCopy.children[i]), newEnt);

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
		usize copyChildCount = nodeCopy.children.size();
		for (usize i = 0; i < copyChildCount; i++)
			CloneChild(findEntity(nodeCopy.children[i]), newEnt);
		
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

	void Scene::transformChildren()
	{
		auto view = mRegistry.view<ChildControllerComponent, NodeComponent>();
		for (auto e : view)
		{
			Entity entity{ e, Ref<Scene>(this) };
			ChildControllerComponent& childController = entity.getComponent<ChildControllerComponent>();
			NodeComponent& node = entity.getComponent<NodeComponent>();

			if (childController.isZero())
				continue;

			for (UUID id : node.children)
			{
				Entity child = findEntity(id);
				if (child.hasComponent<TransformComponent>())
				{
					TransformComponent& transform = child.getComponent<TransformComponent>();
					transform.translation += childController.deltaTranslation;
					transform.rotation += glm::radians(childController.deltaRotation);
					transform.scale += childController.deltaScale;
				}
			}

			childController.zero();
		}
	}

	void Scene::reloadMaps()
	{
		mRegistry.view<TilemapControllerComponent>().each([this](auto e, auto& tmcComponent)
		{
			Ref<Tilemap> tilemap = AssetManager::GetAsset<Tilemap>(tmcComponent.tilemapHandle);

			Entity entity{ e, Ref<Scene>(this) };
			entity.addOrReplaceComponent<ScriptComponent>("Labyrinth.Tilemap");

			Entity tiles = getChildByTag("Tiles", entity);
			if (tiles)
				tiles.removeChildren();
			else
				tiles = CreateEntity("Tiles", entity);

			if (!tilemap)
			{
				LAB_CORE_WARN("Tilemap failed to load");
				return;
			}

			for (i32 y = 0; y < tilemap->getHeight(); y++)
			{
				for (i32 x = 0; x < tilemap->getWidth(); x++)
				{
					std::string tileName = fmt::format("({1}, {2})", x, y);
					Entity tileEntity = CreateEntity(tileName, tiles);
					tileEntity.addComponent<ScriptComponent>("Labyrinth.Tile");
					tmcComponent.tileBehaviour[{x, y}] = tileEntity.getUUID();
				}
			}
		});
	}

	void Scene::DestroyEntityR(Entity entity, Entity parent, bool linkChildren)
	{
		// Set the parent of all entity's children (will be null entity if no parent)
		auto& children = entity.getChildren();
		for (auto& child : children)
		{
			Entity childEnt = findEntity(child);
			if (linkChildren)
			{
				if (parent)
					childEnt.setParent(parent);
				else
					DestroyEntityR(childEnt, entity);
			}
			else DestroyEntityR(childEnt, entity);
		}

		mEntityMap.erase(entity.getUUID());
		mRegistry.destroy(entity);
	}

	void Scene::OnPhysicsStart()
	{
		mPhysicsWorld = new b2World({ 0.0f, -9.81f });

		mRegistry.view<TransformComponent, RigidBodyComponent>().each([this](auto e, const auto& trComponent, auto& rbComponent)
		{
			Entity entity(e, Ref<Scene>(this));
			UUID entityID = entity.getComponent<IDComponent>().id;

			b2BodyDef bodyDef;
			bodyDef.type = BodyTypeToBox2D(rbComponent.type);
			bodyDef.position.Set(trComponent.translation.x, trComponent.translation.y);
			bodyDef.angle = trComponent.rotation.z;
			bodyDef.fixedRotation = rbComponent.fixedRotation;

			b2Body* body = mPhysicsWorld->CreateBody(&bodyDef);
			b2MassData massData = body->GetMassData();;
			massData.mass = rbComponent.mass;
			body->SetMassData(&massData);
			body->SetGravityScale(rbComponent.gravityScale);
			body->SetLinearDamping(rbComponent.linearDrag);
			body->SetAngularDamping(rbComponent.angularDrag);
			body->GetUserData().pointer = (uintptr_t)entityID;

			rbComponent.runtimeBody = body;
		});

		mRegistry.view<TransformComponent, BoxColliderComponent, RigidBodyComponent>().each([this](auto e, const auto& trComponent, auto& bcComponent, const auto& rbComponent)
		{
			Entity entity = { e, this };

			LAB_CORE_ASSERT(rbComponent.runtimeBody);
			b2Body* body = static_cast<b2Body*>(rbComponent.runtimeBody);

			b2PolygonShape polygonShape;
			polygonShape.SetAsBox(trComponent.scale.x * bcComponent.halfExtents.x, trComponent.scale.y * bcComponent.halfExtents.y);

			b2FixtureDef fixtureDef;
			fixtureDef.shape = &polygonShape;
			fixtureDef.density = bcComponent.density;
			fixtureDef.friction = bcComponent.friction;
			body->CreateFixture(&fixtureDef);
		});

		mRegistry.view<TransformComponent, CircleColliderComponent, RigidBodyComponent>().each([this](auto e, const auto& trComponent, auto& ccComponent, const auto& rbComponent)
		{
			Entity entity = { e, this };

			LAB_CORE_ASSERT(rbComponent.runtimeBody);
			b2Body* body = static_cast<b2Body*>(rbComponent.runtimeBody);

			b2CircleShape circleShape;
			circleShape.m_radius = trComponent.scale.x * ccComponent.radius;

			b2FixtureDef fixtureDef;
			fixtureDef.shape = &circleShape;
			fixtureDef.density = ccComponent.density;
			fixtureDef.friction = ccComponent.friction;
			body->CreateFixture(&fixtureDef);
		});
	}

	void Scene::OnPhysicsStop()
	{
		delete mPhysicsWorld;
		mPhysicsWorld = nullptr;
	}

	void Scene::BuildScene()
	{
		mRegistry.group<SpriteRendererComponent>(entt::get<TransformComponent>).each([this](auto entity, const auto& srComponent, const auto& trComponent)
		{
			mRenderStack->addQuad(trComponent, srComponent, Cast<i32>(entity));
		});
		mRegistry.group<CircleRendererComponent>(entt::get<TransformComponent>).each([this](auto entity, const auto& crComponent, const auto& trComponent)
		{
			mRenderStack->addCircle(trComponent, crComponent, Cast<i32>(entity));
		});
	}

	void Scene::DrawScene(EditorCamera& camera)
	{
		Renderer2D::BeginState(camera);
		mRenderStack->draw();
		Renderer2D::EndState();

		mRenderStack->clearItems();
	}

	void Scene::DrawScene(Camera& camera, const glm::mat4& transform)
	{
		Renderer2D::BeginState(camera, transform);
		mRenderStack->draw();
		Renderer2D::EndState();

		mRenderStack->clearItems();
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

	void Scene::UpdateScripts(Timestep ts)
	{
		// Update scripts
		mRegistry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
		{
			if (!nsc.complete && !nsc.instance)
			{
				nsc.instance = nsc.instantiateScript();
				nsc.instance->mScriptEntity = { entity, Ref<Scene>(this) };
				nsc.instance->onStart();
			}

			nsc.instance->onUpdate(ts);

			if (nsc.instance->isComplete())
			{
				nsc.complete = true;
				nsc.instance->onStop();
				nsc.destroyScript();
			}
		});

		mRegistry.view<ScriptComponent>().each([=](auto entity, auto& sc)
		{
			sc.instance->onUpdate(ts);
		});
	}

	Entity Scene::findEntity(UUID findID)
	{
		if (mEntityMap.count(findID) == 0) return Entity();

		return { mEntityMap.at(findID), Ref<Scene>(this) };
	}

	Entity Scene::getEntityByTag(const std::string& tag)
	{
		LAB_PROFILE_FUNCTION();

		auto entities = mRegistry.view<TagComponent>();
		for (auto e : entities)
		{
			if (entities.get<TagComponent>(e).tag == tag)
				return Entity(e, Ref<Scene>(this));
		}

		return Entity{};
	}

	Entity Scene::getChildByTag(const std::string& tag, Entity parent)
	{
		LAB_PROFILE_FUNCTION();

		for (auto e : parent.getChildren())
		{
			Entity entity = findEntity(e);
			if (entity.getComponent<TagComponent>().tag == tag)
				return entity;

			Entity checkChildren = getChildByTag(tag, entity);
			if (checkChildren)
				return checkChildren;
		}

		return Entity{};
	}

	void Scene::getSheetsInUse(std::vector<Ref<Texture2DSheet>>& sheets)
	{
		sheets.clear();
		mRegistry.view<SpriteRendererComponent>().each([&sheets](auto entity, auto& srComponent)
		{
			if (srComponent.type == SpriteRendererComponent::TexType::SubTexture)
			{
				Ref<Texture2DSheet> texSheet = AssetManager::GetAsset<SubTexture2D>(srComponent.handle)->getSheet();
				if (std::find_if(sheets.begin(), sheets.end(),
					[&texSheet](const Ref<Texture2DSheet>& match) {
						return texSheet == match;
					})
					== sheets.end())
					sheets.emplace_back(texSheet);
			}
		});
	}

	void Scene::onRuntimeStart()
	{
		OnPhysicsStart();

		ScriptEngine::SetContext(Ref<Scene>(this));
		ScriptEngine::OnRuntimeStart();
	}

	void Scene::onRuntimeStop()
	{
		OnPhysicsStop();

		ScriptEngine::OnRuntimeStop();
		ScriptEngine::SetContext(nullptr);
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
		UpdateScripts(ts);
		StepPhysics2D(ts);

		Entity camera = getPrimaryCameraEntity();
		if (!camera) return;

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

	void Scene::onViewportResize(u32 width, u32 height)
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
				primaryCam = { entity, Ref<Scene>(this)};
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

	template<>
	void Scene::onComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{
	}

	template<>
	void Scene::onComponentAdded<ChildControllerComponent>(Entity entity, ChildControllerComponent& component)
	{
	}

	template<>
	void Scene::onComponentAdded<ScriptComponent>(Entity entity, ScriptComponent& component)
	{
	}

	template<>
	void Scene::onComponentAdded<TilemapControllerComponent>(Entity entity, TilemapControllerComponent& component)
	{
		reloadMaps();
	}

}
