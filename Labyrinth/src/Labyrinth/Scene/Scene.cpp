#include "Lpch.h"
#include "Scene.h"

#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_circle_shape.h>
#include <box2d/b2_chain_shape.h>

#include <Labyrinth/Animation/Animation.h>
#include <Labyrinth/Assets/AssetManager.h>
#include <Labyrinth/Physics/ContactListener.h>
#include <Labyrinth/Physics/Utils.h>
#include <Labyrinth/Renderer/Renderer.h>
#include <Labyrinth/Renderer/Renderer2D.h>
#include <Labyrinth/Scripting/ScriptEngine.h>
#include <Labyrinth/Tilemap/Tilemap.h>

#include "Entity.h"
#include "Groups.h"

namespace Laby {

	Ref<Scene> SceneUtils::Clone(Ref<Scene> toCopy)
	{
		Ref<Scene> newScene = Ref<Scene>::Create(toCopy->mName);
		newScene->mViewportWidth = toCopy->mViewportWidth;
		newScene->mViewportHeight = toCopy->mViewportHeight;

		std::unordered_map<UUID, EntityID> entMap;

		toCopy->mRegistry.view<IDComponent, TagComponent>().each([&](auto e, const auto& idComp, const auto& tagComp)
		{
			entMap[idComp] = newScene->CreateEntityWithID(idComp, tagComp).getEntID();
		});

		CopyAllComponents(toCopy->mRegistry, newScene->mRegistry, entMap);

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

	Scene::Scene(const std::string& name)
		: mName(name),
		  mGroups(MakeSingle<ECS::Groups>(this)),
		  mRenderStack(MakeSingle<RenderStack>()),
		  mBufferStack(MakeSingle<RenderStack>())
	{
		mSceneEntity = mRegistry.create();

		Box2DWorldComponent& b2dWorld = mRegistry.emplace<Box2DWorldComponent>(mSceneEntity);
		b2dWorld.world = new b2World({ 0.0f, -9.81f });
		b2dWorld.contactListener = new ContactListener;
		b2dWorld.world->SetContactListener(b2dWorld.contactListener);

		mRegistry.on_construct<RigidBodyComponent>().connect<&Scene::OnRigidBodyComponentConstruct>(this);
		mRegistry.on_destroy<RigidBodyComponent>().connect<&Scene::OnRigidBodyComponentDestroy>(this);
		mRegistry.on_construct<BoxColliderComponent>().connect<&Scene::OnBoxColliderComponentConstruct>(this);
		mRegistry.on_destroy<BoxColliderComponent>().connect<&Scene::OnBoxColliderComponentDestroy>(this);
		mRegistry.on_construct<CircleColliderComponent>().connect<&Scene::OnCircleColliderComponentConstruct>(this);
		mRegistry.on_destroy<CircleColliderComponent>().connect<&Scene::OnCircleColliderComponentDestroy>(this);
		mRegistry.on_construct<ChainColliderComponent>().connect<&Scene::OnChainColliderComponentConstruct>(this);
		mRegistry.on_destroy<ChainColliderComponent>().connect<&Scene::OnChainColliderComponentDestroy>(this);

		mStackSwapCallbackID = Renderer::AddFrameCallback([this] { std::swap(mRenderStack, mBufferStack); });
	}

	Scene::~Scene()
	{
		Renderer::RemoveFrameCallback(mStackSwapCallbackID);

		mRegistry.on_construct<RigidBodyComponent>().disconnect();
		mRegistry.on_destroy<RigidBodyComponent>().disconnect();
		mRegistry.on_construct<BoxColliderComponent>().disconnect();
		mRegistry.on_destroy<BoxColliderComponent>().disconnect();
		mRegistry.on_construct<CircleColliderComponent>().disconnect();
		mRegistry.on_destroy<CircleColliderComponent>().disconnect();
		mRegistry.on_construct<ChainColliderComponent>().disconnect();
		mRegistry.on_destroy<ChainColliderComponent>().disconnect();

		Box2DWorldComponent& b2dWorld = mRegistry.get<Box2DWorldComponent>(mSceneEntity);
		delete b2dWorld.world;
		delete b2dWorld.contactListener;
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateEntityWithID(UUID(), name, Entity());
	}

	Entity Scene::CreateEntity(const std::string& name, Entity parent)
	{
		return CreateEntityWithID(UUID(), name, parent);
	}

	Entity Scene::CreateEntityWithID(UUID id, const std::string& name)
	{
		return CreateEntityWithID(id, name, Entity());
	}

	Entity Scene::CreateEntityWithID(UUID id, const std::string& name, Entity parent)
	{
		Entity newEnt(mRegistry.create(), Ref<Scene>(this));

		newEnt.addComponent<IDComponent>().id = id;
		newEnt.addComponent<TransformComponent>();

		newEnt.addComponent<RootComponent>();
		auto& node = newEnt.addComponent<NodeComponent>();
		newEnt.setParent(parent, node);

		auto& tag = newEnt.addComponent<TagComponent>();
		tag = name.empty() ? "Entity" : name;

		mEntityMap[id] = (EntityID)newEnt.getEntID();

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
		Entity parent = entity.getParent();

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
		mRegistry.destroy(entity.getEntID());
	}

	Entity Scene::findEntity(UUID findID)
	{
		if (!mEntityMap.contains(findID))
			return Entity();

		return { mEntityMap.at(findID), Ref<Scene>(this) };
	}

	Entity Scene::getEntityByTag(std::string_view tag)
	{
		auto entities = mRegistry.view<TagComponent>();
		for (auto e : entities)
		{
			if (entities.get<TagComponent>(e).tag == tag)
				return Entity(e, Ref<Scene>(this));
		}

		return Entity{};
	}

	Entity Scene::getChildByTag(std::string_view tag, Entity parent)
	{
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

	void Scene::onRuntimeStart()
	{
		CreateTilemapEntities();
		ResetAnimations(true);

		ScriptEngine::SetContext(Ref<Scene>(this));
		ScriptEngine::OnRuntimeStart();
	}

	void Scene::onRuntimeStop()
	{
		CleanupTilemapEntities();
		ResetAnimations();

		ScriptEngine::OnRuntimeStop();
		ScriptEngine::SetContext(nullptr);
	}

	void Scene::onSimulationStart()
	{
		CreateTilemapEntities();
		ResetAnimations(true);
	}

	void Scene::onSimulationStop()
	{
		CleanupTilemapEntities();
		ResetAnimations();
	}

	void Scene::onUpdateRuntime(Timestep ts)
	{
		UpdateScripts(ts);
		StepPhysics2D(ts);
		UpdateAnimation();

		Entity camera = getPrimaryCameraEntity();
		if (!camera)
		{
			LAB_CORE_WARN("No camera in scene to use!");
			return;
		}

		BuildScene();
		DrawScene(camera.getComponent<CameraComponent>(), camera.getTransform());
	}

	void Scene::onUpdateSimulation(Timestep ts, EditorCamera& camera)
	{
		StepPhysics2D(ts);
		UpdateAnimation();

		BuildScene();
		DrawScene(camera);
	}

	void Scene::onUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		UpdateAnimation();

		BuildScene();
		DrawScene(camera);
	}

	void Scene::onViewportResize(u32 width, u32 height)
	{
		mViewportWidth = width;
		mViewportHeight = height;

		// Resize our non-FixedAspectRatio cameras
		mRegistry.view<CameraComponent>().each([this](auto entity, auto& cameraComponent)
		{
			if (!cameraComponent.fixedAspectRatio)
				cameraComponent.camera.setViewportSize(mViewportWidth, mViewportHeight);
		});
	}

	Entity Scene::getPrimaryCameraEntity()
	{
		Entity primaryCam;
		mRegistry.view<CameraComponent>().each([this, &primaryCam](auto entity, const auto& cameraComponent) 
		{
			if (cameraComponent.primary)
				primaryCam = { entity, Ref<Scene>(this) };
		});
		return primaryCam;
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
					TransformComponent& transform = child.getTransform();
					transform.translation += childController.deltaTranslation;
					transform.rotation += glm::radians(childController.deltaRotation);
					transform.scale += childController.deltaScale;
				}
			}

			childController.zero();
		}
	}

	void Scene::CreateTilemapEntities()
	{
		mRegistry.view<TilemapComponent>().each([this](auto e, const auto& tilemapComponent)
		{
			Entity mapEntity = { e, this };
			CreateTilemapShapes(mapEntity, tilemapComponent);
			CreateTilemapScripts(mapEntity, tilemapComponent);
		});
	}

	void Scene::CreateTilemapShapes(Entity mapEntity, const TilemapComponent& comp)
	{
		Ref<Tilemap> tilemap = AssetManager::GetAsset<Tilemap>(comp.mapHandle);
		if (!tilemap)
		{
			LAB_CORE_WARN("Tilemap failed to load or there was no tilemap.");
			return;
		}

		const std::string& mapName = tilemap->getName();
		usize width = tilemap->getWidth();
		usize height = tilemap->getHeight();

		const auto& mapTransform = mapEntity.getTransform();
		glm::vec3 tileSize = glm::vec3{ mapTransform.scale.x / width, mapTransform.scale.y / height, 1.0f };

		Entity shapesEntity = CreateEntity("Shapes", mapEntity);
		shapesEntity.removeComponent<TransformComponent>();

		usize i = 1;
		for (const ChainShape& shape : tilemap->getPhysicsShapes())
		{
			Entity shapeEntity = CreateEntity(fmt::format("{}-Shape{}", mapName, i), shapesEntity);
			auto& shapeTransform = shapeEntity.getTransform();

			const glm::vec2& centroid = shape.centroid();
			const glm::vec2& extents = shape.extents;

			shapeTransform.translation.x = tileSize.x * (centroid.x - 0.5f * (f32)(width - 1));
			shapeTransform.translation.y = tileSize.y * (0.5f * (f32)(height - 1) - centroid.y);
			shapeTransform.scale = { tileSize.x * extents.x, tileSize.y * extents.y, tileSize.z };

			shapeEntity.addComponent<RigidBodyComponent>();
			shapeEntity.addComponent<ChainColliderComponent>(shape);
		}
	}

	void Scene::CreateTilemapScripts(Entity mapEntity, const TilemapComponent& comp)
	{
		Ref<Tilemap> tilemap = AssetManager::GetAsset<Tilemap>(comp.mapHandle);
		if (!tilemap)
		{
			LAB_CORE_WARN("Tilemap failed to load or there was no tilemap.");
			return;
		}

		const std::string& mapName = tilemap->getName();
		usize width = tilemap->getWidth();
		usize height = tilemap->getHeight();

		const auto& mapTransform = mapEntity.getTransform();
		glm::vec3 tileSize = glm::vec3{ mapTransform.scale.x / width, mapTransform.scale.y / height, 1.0f };

		Entity scriptsEntity = CreateEntity("Scripts", mapEntity);
		scriptsEntity.removeComponent<TransformComponent>();

		for (const TileScriptData& tileData : tilemap->getTileScripts())
		{
			Entity scriptEntity = CreateEntity(fmt::format("{}-Script({}, {} - {})", mapName, tileData.pos.x, tileData.pos.y, tileData.script), scriptsEntity);
			auto& scriptTransform = scriptEntity.getTransform();

			scriptTransform.translation.x = tileSize.x * (tileData.pos.x - 0.5f * (f32)(width - 1));
			scriptTransform.translation.y = tileSize.y * (0.5f * (f32)(height - 1) - tileData.pos.y);
			scriptTransform.scale = tileSize;

			scriptEntity.addComponent<ScriptComponent>(tileData.script);
			scriptEntity.addComponent<RigidBodyComponent>();
			auto& collider = scriptEntity.addComponent<BoxColliderComponent>();
			collider.sensor = true;
		}
	}

	void Scene::CleanupTilemapEntities()
	{
		mRegistry.view<TilemapComponent>().each([this](auto e, const auto& tilemapComponent)
		{
			Entity mapEntity = { e, this };

			Entity shapesEntity = getChildByTag("Shapes", mapEntity);
			if (shapesEntity)
				DestroyEntity(shapesEntity);

			Entity scriptsEntity = getChildByTag("Scripts", mapEntity);
			if (scriptsEntity)
				DestroyEntity(scriptsEntity);
		});
	}

	void Scene::ResetAnimations(bool start)
	{
		mGroups->each<ECS::Animations>([start](auto& ac, auto& src)
		{
			Ref<Animation> animation = AssetManager::GetAsset<Animation>(ac.handle);
			if (!animation)
				return;

			ac.frameCounter = 0;
			ac.frameIndex = 0;
			ac.playing = start;

			src.handle = animation->getFrame(ac.frameIndex).sprite;
			src.type = RenderType::SubTexture;
		});
	}

	void Scene::BuildScene()
	{
		mBufferStack->clearItems();
		mGroups->each<ECS::Sprites>([this](auto entity, const auto& srComponent, const auto& trComponent)
		{
			mBufferStack->addQuad(trComponent, srComponent, (i32)entity);
		});
		mGroups->each<ECS::Circles>([this](auto entity, const auto& crComponent, const auto& trComponent)
		{
			mBufferStack->addCircle(trComponent, crComponent, (i32)entity);
		});
	}

	void Scene::DrawScene(EditorCamera& camera)
	{
		Renderer::Submit([&, this]()
		{
			Renderer2D::BeginState(camera);
			mRenderStack->draw();
			Renderer2D::EndState();
		});
	}

	void Scene::DrawScene(Camera& camera, const glm::mat4& transform)
	{
		Renderer::Submit([&, this]()
		{
			Renderer2D::BeginState(camera, transform);
			mRenderStack->draw();
			Renderer2D::EndState();
		});
	}

	void Scene::StepPhysics2D(Timestep ts)
	{
		// Physics
		Entity sceneEntity(mSceneEntity, Ref<Scene>(this));
		auto* world = sceneEntity.getComponent<Box2DWorldComponent>().world;
		if (!world)
			return;

		const int32_t velIters = 6;
		const int32_t poslIters = 2;
		world->Step(ts, velIters, poslIters);

		mGroups->each<ECS::Bodies>([this](auto entity, auto& rbComponent, auto& trComponent)
		{
			b2Body* body = StaticCast<b2Body>(rbComponent.runtimeBody);
			const auto& pos = body->GetPosition();
			trComponent.translation.x = pos.x;
			trComponent.translation.y = pos.y;
			trComponent.rotation.z = body->GetAngle();
		});
	}

	void Scene::UpdateScripts(Timestep ts)
	{
		mRegistry.view<ScriptComponent>().each([=](auto entity, auto& sc)
		{
			if (sc.instance) 
				sc.instance->onUpdate(ts);
		});
	}

	void Scene::UpdateAnimation()
	{
		mGroups->each<ECS::Animations>([](auto e, auto& ac, auto& src)
		{
			if (!ac.playing)
				return;

			Ref<Animation> animation = AssetManager::GetAsset<Animation>(ac.handle);
			if (!animation)
				return;

			const auto& frame = animation->getFrame(ac.frameIndex);
			if (++ac.frameCounter < frame.length)
				return;

			if (++ac.frameIndex == animation->getFrameCount())
			{
				ac.frameIndex = 0;
				ac.playing = !ac.playOnce;
			}

			ac.frameCounter = 0;
			src.type = RenderType::SubTexture;
			src.handle = frame.sprite;
		});
	}

	void Scene::OnRigidBodyComponentConstruct(entt::registry& registry, entt::entity e)
	{
		Entity sceneEntity(mSceneEntity, Ref<Scene>(this));
		auto* world = sceneEntity.getComponent<Box2DWorldComponent>().world;

		Entity entity(e, Ref<Scene>(this));
		UUID entityID = entity.getComponent<IDComponent>().id;
		TransformComponent& transform = entity.getComponent<TransformComponent>();
		auto& rbComponent = entity.getComponent<RigidBodyComponent>();

		b2BodyDef bodyDef;
		bodyDef.type = PhysicsUtils::BodyTypeToBox2D(rbComponent.type);
		bodyDef.position.Set(transform.translation.x, transform.translation.y);
		bodyDef.angle = transform.rotation.z;
		bodyDef.fixedRotation = rbComponent.fixedRotation;

		b2Body* body = world->CreateBody(&bodyDef);
		b2MassData massData = body->GetMassData();;
		massData.mass = rbComponent.mass;
		body->SetMassData(&massData);
		body->SetGravityScale(rbComponent.gravityScale);
		body->SetLinearDamping(rbComponent.linearDrag);
		body->SetAngularDamping(rbComponent.angularDrag);
		body->GetUserData().pointer = (uintptr_t)entityID;

		rbComponent.runtimeBody = body;
	}

	void Scene::OnRigidBodyComponentDestroy(entt::registry& registry, entt::entity e)
	{
	}

	void Scene::OnBoxColliderComponentConstruct(entt::registry& registry, entt::entity e)
	{
		Entity sceneEntity(mSceneEntity, Ref<Scene>(this));
		auto* world = sceneEntity.getComponent<Box2DWorldComponent>().world;

		Entity entity(e, Ref<Scene>(this));
		UUID entityID = entity.getComponent<IDComponent>().id;
		TransformComponent& trComponent = entity.getComponent<TransformComponent>();
		auto& rbComponent = entity.getComponent<RigidBodyComponent>();
		auto& bcComponent = entity.getComponent<BoxColliderComponent>();

		LAB_CORE_ASSERT(rbComponent.runtimeBody);
		b2Body* body = StaticCast<b2Body>(rbComponent.runtimeBody);

		b2PolygonShape polygonShape;
		polygonShape.SetAsBox(trComponent.scale.x * bcComponent.halfExtents.x, trComponent.scale.y * bcComponent.halfExtents.y);

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &polygonShape;
		fixtureDef.density = bcComponent.density;
		fixtureDef.friction = bcComponent.friction;
		fixtureDef.isSensor = bcComponent.sensor;
		bcComponent.runtimeFixture = body->CreateFixture(&fixtureDef);
	}

	void Scene::OnBoxColliderComponentDestroy(entt::registry& registry, entt::entity e)
	{
	}

	void Scene::OnCircleColliderComponentConstruct(entt::registry& registry, entt::entity e)
	{
		Entity sceneEntity(mSceneEntity, Ref<Scene>(this));
		auto* world = sceneEntity.getComponent<Box2DWorldComponent>().world;

		Entity entity(e, Ref<Scene>(this));
		UUID entityID = entity.getComponent<IDComponent>().id;
		TransformComponent& trComponent = entity.getComponent<TransformComponent>();
		auto& rbComponent = entity.getComponent<RigidBodyComponent>();
		auto& ccComponent = entity.getComponent<CircleColliderComponent>();

		LAB_CORE_ASSERT(rbComponent.runtimeBody);
		b2Body* body = StaticCast<b2Body>(rbComponent.runtimeBody);

		b2CircleShape circleShape;
		circleShape.m_radius = trComponent.scale.x * ccComponent.radius;

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &circleShape;
		fixtureDef.density = ccComponent.density;
		fixtureDef.friction = ccComponent.friction;
		fixtureDef.isSensor = ccComponent.sensor;
		ccComponent.runtimeFixture = body->CreateFixture(&fixtureDef);
	}

	void Scene::OnCircleColliderComponentDestroy(entt::registry& registry, entt::entity e)
	{
	}

	void Scene::OnChainColliderComponentConstruct(entt::registry& registry, entt::entity e)
	{
		Entity sceneEntity(mSceneEntity, Ref<Scene>(this));
		auto* world = sceneEntity.getComponent<Box2DWorldComponent>().world;

		Entity entity(e, Ref<Scene>(this));
		UUID entityID = entity.getComponent<IDComponent>().id;
		TransformComponent& trComponent = entity.getComponent<TransformComponent>();
		auto& rbComponent = entity.getComponent<RigidBodyComponent>();
		auto& ccComponent = entity.getComponent<ChainColliderComponent>();

		LAB_CORE_ASSERT(rbComponent.runtimeBody);
		b2Body* body = StaticCast<b2Body>(rbComponent.runtimeBody);

		LAB_CORE_ASSERT(ccComponent.vertexCount);
		b2ChainShape chainShape;
		chainShape.CreateLoop(ccComponent.getVertices(), ccComponent.vertexCount);

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &chainShape;
		fixtureDef.density = ccComponent.density;
		fixtureDef.friction = ccComponent.friction;
		fixtureDef.isSensor = ccComponent.sensor;
		ccComponent.runtimeFixture = body->CreateFixture(&fixtureDef);
	}

	void Scene::OnChainColliderComponentDestroy(entt::registry& registry, entt::entity e)
	{
	}
}
