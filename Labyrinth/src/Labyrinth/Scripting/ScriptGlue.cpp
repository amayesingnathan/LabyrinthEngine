#include "Lpch.h"
#include "ScriptGlue.h"

#include <mono/metadata/class.h>
#include <mono/metadata/object.h>
#include <mono/metadata/reflection.h>
#include <mono/metadata/loader.h>
#include <mono/jit/jit.h>

#include <box2d/box2d.h>

#include "ScriptEngine.h"
#include "MarshalUtils.h"

#include <Labyrinth/Animation/Animation.h>
#include <Labyrinth/Assets/AssetManager.h>
#include <Labyrinth/Containers/Buffer.h>
#include <Labyrinth/IO/Input.h>
#include <Labyrinth/Scene/Scene.h>
#include <Labyrinth/Scene/Entity.h>

namespace Laby {

	static inline Entity GetEntity(UUID entityID)
	{
		Ref<Scene> scene = ScriptEngine::GetContext();
		LAB_CORE_ASSERT(scene, "No active scene!");
		return scene->findEntity(entityID);
	};

	static std::unordered_map<MonoType*, std::function<void(Entity&)>> sCreateComponentFuncs;
	static std::unordered_map<MonoType*, std::function<bool(Entity&)>> sHasComponentFuncs;
	static std::unordered_map<MonoType*, std::function<void(Entity&)>> sRemoveComponentFuncs;

#define LAB_ADD_INTERNAL_CALL(funcName) mono_add_internal_call("Labyrinth.Internal::"#funcName, GlueFunctions::funcName)

#define LAB_REGISTER_COMPONENT(Type) \
		{\
			MonoType* managedType = mono_reflection_type_from_name((char*)"Labyrinth." #Type, ScriptEngineInternal::GetCoreAssemblyInfo()->assemblyImage);\
			if (managedType)\
			{\
				sCreateComponentFuncs[managedType] = [](Entity& entity) { entity.addComponent<Type>(); };\
				sHasComponentFuncs[managedType] = [](Entity& entity) { return entity.hasComponent<Type>(); };\
				sRemoveComponentFuncs[managedType] = [](Entity& entity){ entity.removeComponent<Type>(); };\
			}\
			else\
			{\
				LAB_CORE_ASSERT(false, "No C# component class found for " #Type "!");\
			}\
		}

	void ScriptGlue::Register()
	{
		if (sCreateComponentFuncs.size() > 0)
		{
			sCreateComponentFuncs.clear();
			sHasComponentFuncs.clear();
			sRemoveComponentFuncs.clear();
		}

		RegisterComponentTypes();
		RegisterInternalCalls();
	}

	void ScriptGlue::RegisterComponentTypes()
	{
		LAB_REGISTER_COMPONENT(TagComponent);
		LAB_REGISTER_COMPONENT(TransformComponent);
		LAB_REGISTER_COMPONENT(CameraComponent);
		LAB_REGISTER_COMPONENT(SpriteRendererComponent);
		LAB_REGISTER_COMPONENT(RigidBodyComponent);
		LAB_REGISTER_COMPONENT(BoxColliderComponent);
		LAB_REGISTER_COMPONENT(CircleColliderComponent);
	}

	void ScriptGlue::RegisterInternalCalls()
	{
		LAB_ADD_INTERNAL_CALL(Log_Trace);
		LAB_ADD_INTERNAL_CALL(Log_Info);
		LAB_ADD_INTERNAL_CALL(Log_Warning);
		LAB_ADD_INTERNAL_CALL(Log_Error);
		LAB_ADD_INTERNAL_CALL(Log_Critical);

		LAB_ADD_INTERNAL_CALL(Input_IsKeyPressed);

		LAB_ADD_INTERNAL_CALL(AssetHandle_IsValid);

		LAB_ADD_INTERNAL_CALL(Scene_FindEntityByTag);
		LAB_ADD_INTERNAL_CALL(Scene_IsEntityValid);
		LAB_ADD_INTERNAL_CALL(Scene_CreateEntity);
		LAB_ADD_INTERNAL_CALL(Scene_DestroyEntity);
		LAB_ADD_INTERNAL_CALL(Scene_GetEntities);

		LAB_ADD_INTERNAL_CALL(Entity_GetParent);
		LAB_ADD_INTERNAL_CALL(Entity_SetParent);
		LAB_ADD_INTERNAL_CALL(Entity_GetChildren);
		LAB_ADD_INTERNAL_CALL(Entity_CreateComponent);
		LAB_ADD_INTERNAL_CALL(Entity_HasComponent);
		LAB_ADD_INTERNAL_CALL(Entity_RemoveComponent);

		LAB_ADD_INTERNAL_CALL(Tag_GetTag);
		LAB_ADD_INTERNAL_CALL(Tag_SetTag);

		LAB_ADD_INTERNAL_CALL(Transform_GetTransform);
		LAB_ADD_INTERNAL_CALL(Transform_SetTransform);
		LAB_ADD_INTERNAL_CALL(Transform_GetTranslation);
		LAB_ADD_INTERNAL_CALL(Transform_SetTranslation);
		LAB_ADD_INTERNAL_CALL(Transform_GetRotation);
		LAB_ADD_INTERNAL_CALL(Transform_SetRotation);
		LAB_ADD_INTERNAL_CALL(Transform_GetScale);
		LAB_ADD_INTERNAL_CALL(Transform_SetScale);

		LAB_ADD_INTERNAL_CALL(Camera_GetVerticalFOV);
		LAB_ADD_INTERNAL_CALL(Camera_SetVerticalFOV);

		LAB_ADD_INTERNAL_CALL(RigidBody_GetBodyType);
		LAB_ADD_INTERNAL_CALL(RigidBody_SetBodyType);
		LAB_ADD_INTERNAL_CALL(RigidBody_GetTranslation);
		LAB_ADD_INTERNAL_CALL(RigidBody_SetTranslation);
		LAB_ADD_INTERNAL_CALL(RigidBody_GetRotation);
		LAB_ADD_INTERNAL_CALL(RigidBody_SetRotation);
		LAB_ADD_INTERNAL_CALL(RigidBody_GetMass);
		LAB_ADD_INTERNAL_CALL(RigidBody_SetMass);
		LAB_ADD_INTERNAL_CALL(RigidBody_GetLinearVelocity);
		LAB_ADD_INTERNAL_CALL(RigidBody_SetLinearVelocity);
		LAB_ADD_INTERNAL_CALL(RigidBody_GetGravityScale);
		LAB_ADD_INTERNAL_CALL(RigidBody_SetGravityScale);
		LAB_ADD_INTERNAL_CALL(RigidBody_ApplyLinearImpulse);
		LAB_ADD_INTERNAL_CALL(RigidBody_ApplyAngularImpulse);
		LAB_ADD_INTERNAL_CALL(RigidBody_AddForce);
		LAB_ADD_INTERNAL_CALL(RigidBody_AddTorque);
	}


#pragma region Logging

	void GlueFunctions::Log_Trace(MonoString* string)
	{
		char* cStr = mono_string_to_utf8(string);
		LAB_CORE_TRACE(cStr);
		mono_free(cStr);
	}
	void GlueFunctions::Log_Info(MonoString* string)
	{
		char* cStr = mono_string_to_utf8(string);
		LAB_CORE_INFO(cStr);
		mono_free(cStr);
	}
	void GlueFunctions::Log_Warning(MonoString* string)
	{
		char* cStr = mono_string_to_utf8(string);
		LAB_CORE_WARN(cStr);
		mono_free(cStr);
	}
	void GlueFunctions::Log_Error(MonoString* string)
	{
		char* cStr = mono_string_to_utf8(string);
		LAB_CORE_ERROR(cStr);
		mono_free(cStr);
	}
	void GlueFunctions::Log_Critical(MonoString* string)
	{
		char* cStr = mono_string_to_utf8(string);
		LAB_CORE_CRITICAL(cStr);
		mono_free(cStr);
	}
#pragma endregion

#pragma region Input
	bool GlueFunctions::Input_IsKeyPressed(KeyCode key)
	{
		return Input::IsKeyPressed(key);
	}
#pragma endregion

#pragma region AssetHandle

	bool GlueFunctions::AssetHandle_IsValid(AssetHandle assetHandle)
	{
		return AssetManager::IsAssetHandleValid(assetHandle);
	}

#pragma endregion

#pragma region Scene

	u64 GlueFunctions::Scene_FindEntityByTag(MonoString* tag)
	{
		Ref<Scene> scene = ScriptEngine::GetContext();
		LAB_CORE_ASSERT(scene, "No active scene!");
		Entity entity = scene->getEntityByTag(MarshalUtils::MonoStringToUTF8(tag));

		if (!entity)
			return 0;

		return entity.getUUID();
	}

	bool GlueFunctions::Scene_IsEntityValid(UUID entityID)
	{
		if (!entityID)
			return false;

		return (bool)(ScriptEngine::GetContext()->findEntity(entityID));
	}

	u64 GlueFunctions::Scene_CreateEntity(MonoString* tag)
	{
		Ref<Scene> scene = ScriptEngine::GetContext();
		LAB_CORE_ASSERT(scene, "No active scene!");
		return scene->CreateEntity(MarshalUtils::MonoStringToUTF8(tag)).getUUID();
	}

	void GlueFunctions::Scene_DestroyEntity(UUID entityID)
	{
		Entity entity = GetEntity(entityID);
		if (!entity)
			return;

		Ref<Scene> scene = ScriptEngine::GetContext();
		scene->DestroyEntity(entity);
	}

	MonoArray* GlueFunctions::Scene_GetEntities()
	{
		Ref<Scene> scene = ScriptEngine::GetContext();
		LAB_CORE_ASSERT(scene, "No active scene!");

		auto entities = scene->getEntitiesWith<IDComponent>();
		auto entityClass = ScriptEngineInternal::GetCoreEntityClass();
		MonoArray* result = mono_array_new(ScriptEngineInternal::GetAppDomain(), entityClass->getClass(), entities.size());
		u32 i = 0;
		entities.each([&i, result, &entityClass](auto entity, const auto& idComp)
			{
				MonoObject* boxed = entityClass->instantiate(idComp.id);
		mono_array_setref(result, i++, boxed);
			});

		return result;
	}

#pragma endregion

#pragma region Entity

	UUID GlueFunctions::Entity_GetParent(UUID entityID)
	{
		auto entity = GetEntity(entityID);
		if (!entity)
			return 0;
		return entity.getParent().getUUID();
	}

	void GlueFunctions::Entity_SetParent(UUID entityID, UUID parentID)
	{
		Entity child = GetEntity(entityID);
		Entity parent = GetEntity(parentID);

		if (!child)
		{
			LAB_CORE_ERROR("Entity.SetParent - Invalid entity!");
			return;
		}

		if (!parent)
		{
			LAB_CORE_ERROR("Entity.SetParent - Invalid parent entity!");
			return;
		}

		child.setParent(parent);
	}

	MonoArray* GlueFunctions::Entity_GetChildren(u64 entityID)
	{
		Entity entity = GetEntity(entityID);
		if (!entity)
		{
			LAB_CORE_ERROR("Entity.GetChildren - Invalid entity!");
			return nullptr;
		}

		const auto& children = entity.getChildren();
		auto entityClass = ScriptEngineInternal::GetCoreEntityClass();
		MonoArray* result = mono_array_new(ScriptEngineInternal::GetAppDomain(), entityClass->getClass(), children.size());
		for (uint32_t i = 0; i < children.size(); i++)
		{
			MonoObject* boxed = entityClass->instantiate(children[i]);
			mono_array_setref(result, i, boxed);
		}

		return result;
	}

	void GlueFunctions::Entity_CreateComponent(u64 entityID, MonoReflectionType* reflectionType)
	{
		auto entity = GetEntity(entityID);
		if (!entity)
		{
			LAB_CORE_ERROR("Entity.CreateComponent - Invalid entity!");
			return;
		}

		if (reflectionType == nullptr)
		{
			LAB_CORE_ERROR("Entity.CreateComponent - Cannot create a null component!");
			return;
		}

		MonoType* managedType = mono_reflection_type_get_type(reflectionType);
		if (sCreateComponentFuncs.find(managedType) == sCreateComponentFuncs.end())
		{
			char* typeName = mono_type_get_name(managedType);
			LAB_CORE_ERROR("Entity.CreateComponent - Unknown component type {0}!", typeName);
			mono_free(typeName);
			return;
		}

		sCreateComponentFuncs.at(managedType)(entity);
	}

	bool GlueFunctions::Entity_HasComponent(u64 entityID, MonoReflectionType* reflectionType)
	{
		auto entity = GetEntity(entityID);
		if (!entity)
		{
			LAB_CORE_ERROR("Entity.HasComponent - Invalid entity!");
			return false;
		}

		if (reflectionType == nullptr)
		{
			LAB_CORE_ERROR("Entity.HasComponent - Entities can never have a null component!");
			return false;
		}

		MonoType* managedType = mono_reflection_type_get_type(reflectionType);
		if (sHasComponentFuncs.find(managedType) == sHasComponentFuncs.end())
		{
			char* typeName = mono_type_get_name(managedType);
			LAB_CORE_ERROR("Entity.HasComponent - Unknown component type {0}!", typeName);
			mono_free(typeName);
			return false;
		}

		return sHasComponentFuncs.at(managedType)(entity);
	}

	bool GlueFunctions::Entity_RemoveComponent(u64 entityID, MonoReflectionType* componentType)
	{
		auto entity = GetEntity(entityID);
		if (!entity)
		{
			LAB_CORE_ERROR("Entity.RemoveComponent called on an invalid Entity!");
			return false;
		}

		if (componentType == nullptr)
		{
			LAB_CORE_ERROR("Tried to call Entity.RemoveComponent with a 'null' component!");
			return false;
		}

		MonoType* managedType = mono_reflection_type_get_type(componentType);
		char* typeName = mono_type_get_name(managedType);
		if (sRemoveComponentFuncs.find(managedType) == sRemoveComponentFuncs.end())
		{
			LAB_CORE_ERROR("Entity.RemoveComponent called with an unknown component type '{0}'!", typeName);
			return false;
		}

		if (!sHasComponentFuncs.at(managedType)(entity))
		{
			LAB_CORE_WARN("Tried to remove component '{0}' from Entity '{1}' even though it doesn't have that component.", typeName, entity.getComponent<TagComponent>().tag);
			return false;
		}

		mono_free(typeName);
		sRemoveComponentFuncs.at(managedType)(entity);
		return true;
	}
#pragma endregion

#pragma region TagComponent
	MonoString* GlueFunctions::Tag_GetTag(UUID id)
	{
		auto entity = GetEntity(id);
		if (!entity)
		{
			LAB_CORE_ERROR("TagComponent.GetTag - Invalid entity!");
			return nullptr;
		}

		const auto& tagComponent = entity.getComponent<TagComponent>();
		return MarshalUtils::UTF8StringToMono(tagComponent.tag);
	}

	void GlueFunctions::Tag_SetTag(UUID id, MonoString* inTag)
	{
		auto entity = GetEntity(id);
		if (!entity)
		{
			LAB_CORE_ERROR("TagComponent.SetTag - Invalid entity!");
			return;
		}

		auto& tagComponent = entity.getComponent<TagComponent>();
		tagComponent.tag = MarshalUtils::MonoStringToUTF8(inTag);
	}
#pragma endregion

#pragma region TransformComponent
	void GlueFunctions::Transform_GetTransform(UUID id, TransformComponent* outTranslation)
	{
		auto entity = GetEntity(id);
		if (!entity)
		{
			LAB_CORE_ERROR("TransformComponent.GetTransform - Invalid entity!");
			return;
		}

		*outTranslation = entity.getTransform();
	}
	void GlueFunctions::Transform_SetTransform(UUID id, TransformComponent* translation)
	{
		auto entity = GetEntity(id);
		if (!entity)
		{
			LAB_CORE_ERROR("TransformComponent.SetTransform - Invalid entity!");
			return;
		}

		entity.getTransform() = *translation;
	}
	void GlueFunctions::Transform_GetTranslation(UUID id, glm::vec3* outTranslation)
	{
		auto entity = GetEntity(id);
		if (!entity)
		{
			LAB_CORE_ERROR("TransformComponent.GetTranslation - Invalid entity!");
			return;
		}

		*outTranslation = entity.getTransform().translation;
	}
	void GlueFunctions::Transform_SetTranslation(UUID id, glm::vec3* translation)
	{
		auto entity = GetEntity(id);
		if (!entity)
		{
			LAB_CORE_ERROR("TransformComponent.SetTranslation - Invalid entity!");
			return;
		}

		entity.getTransform().translation = *translation;
	}
	void GlueFunctions::Transform_GetRotation(UUID id, glm::vec3* rotation)
	{
		auto entity = GetEntity(id);
		if (!entity)
		{
			LAB_CORE_ERROR("TransformComponent.GetRotation- Invalid entity!");
			return;
		}

		*rotation = entity.getTransform().rotation;
	}
	void GlueFunctions::Transform_SetRotation(UUID id, glm::vec3* rotation)
	{
		auto entity = GetEntity(id);
		if (!entity)
		{
			LAB_CORE_ERROR("TransformComponent.SetRotation - Invalid entity!");
			return;
		}

		entity.getTransform().rotation = *rotation;
	}
	void GlueFunctions::Transform_GetScale(UUID id, glm::vec3* outScale)
	{
		auto entity = GetEntity(id);
		if (!entity)
		{
			LAB_CORE_ERROR("TransformComponent.GetScale- Invalid entity!");
			return;
		}

		*outScale = entity.getTransform().scale;
	}
	void GlueFunctions::Transform_SetScale(UUID id, glm::vec3* scale)
	{
		auto entity = GetEntity(id);
		if (!entity)
		{
			LAB_CORE_ERROR("TransformComponent.SetScale- Invalid entity!");
			return;
		}

		entity.getTransform().scale = *scale;
	}
#pragma endregion

#pragma region CameraComponent

	float GlueFunctions::Camera_GetVerticalFOV(UUID entityID)
	{
		Entity entity = GetEntity(entityID);

		if (!entity)
		{
			LAB_CORE_ERROR("CameraComponent.GetVerticalFOV - Invalid entity!");
			return 0.0f;
		}

		if (!entity.hasComponent<CameraComponent>())
		{
			LAB_CORE_ERROR("CameraComponent.GetVerticalFOV - Entity doesn't have a CameraComponent");
			return 0.0f;
		}

		const auto& component = entity.getComponent<CameraComponent>();
		return component.camera.getPerspectiveVerticalFOV();
	}

	void GlueFunctions::Camera_SetVerticalFOV(UUID entityID, float verticalFOV)
	{
		Entity entity = GetEntity(entityID);

		if (!entity)
		{
			LAB_CORE_ERROR("CameraComponent.SetVerticalFOV - Invalid entity!");
			return;
		}

		if (!entity.hasComponent<CameraComponent>())
		{
			LAB_CORE_ERROR("CameraComponent.SetVerticalFOV - Entity doesn't have a CameraComponent");
			return;
		}

		auto& component = entity.getComponent<CameraComponent>();
		return component.camera.setPerspectiveVerticalFOV(verticalFOV);
	}

#pragma endregion

#pragma region ScriptComponent

	MonoObject* GlueFunctions::Script_GetInstance(u64 entityID)
	{
		auto entity = GetEntity(entityID);
		if (!entity)
		{
			LAB_CORE_ERROR("ScriptComponent.Instance - Invalid entity!");
			return nullptr;
		}

		if (!entity.hasComponent<ScriptComponent>())
		{
			LAB_CORE_ERROR("ScriptComponent.Instance - Entity doesn't have a ScriptComponent?");
			return nullptr;
		}

		auto& component = entity.getComponent<ScriptComponent>();

		if (!ScriptEngineInternal::GetAppAssemblyInfo()->classes.count(component.className.data()) != 0)
		{
			LAB_CORE_ERROR("ScriptComponent.Instance - Entity is referencing an invalid C# class!");
			return nullptr;
		}

		if (!component.initialised || !component.instance->valid())
		{
			LAB_CORE_ERROR("ScriptComponent.Instance - Entity '{0}' isn't instantiated?", entity.getComponent<TagComponent>().tag);
			return nullptr;
		}

		return component.instance->obj();
	}
#pragma endregion

#pragma region RigidBodyComponent

	RigidBodyComponent::BodyType GlueFunctions::RigidBody_GetBodyType(u64 entityID)
	{
		auto entity = GetEntity(entityID);
		if (!entity)
		{
			LAB_CORE_ERROR("RigidBodyComponent.GetBodyType - Invalid entity!");
			return RigidBodyComponent::BodyType::None;
		}

		return entity.getComponent<RigidBodyComponent>().type;
	}

	void GlueFunctions::RigidBody_SetBodyType(u64 entityID, RigidBodyComponent::BodyType inType)
	{
		auto entity = GetEntity(entityID);
		if (!entity)
		{
			LAB_CORE_ERROR("RigidBodyComponent.SetBodyType - Invalid entity!");
			return;
		}

		entity.getComponent<RigidBodyComponent>().type = inType;
	}

	void GlueFunctions::RigidBody_GetTranslation(u64 entityID, glm::vec2* outTranslation)
	{
		auto entity = GetEntity(entityID);
		if (!entity)
		{
			LAB_CORE_ERROR("RigidBodyComponent.GetTranslation - Invalid entity!");
			*outTranslation = glm::vec2(0.0f);
			return;
		}

		const auto& component = entity.getComponent<RigidBodyComponent>();
		if (component.runtimeBody == nullptr)
		{
			LAB_CORE_ERROR("RigidBodyComponent.GetTranslation - No b2Body exists for this entity!");
			*outTranslation = glm::vec2(0.0f);
			return;
		}

		b2Body* body = (b2Body*)component.runtimeBody;
		const b2Vec2& translation = body->GetPosition();
		outTranslation->x = translation.x;
		outTranslation->y = translation.y;
	}

	void GlueFunctions::RigidBody_SetTranslation(u64 entityID, glm::vec2* inTranslation)
	{
		auto entity = GetEntity(entityID);
		if (!entity)
		{
			LAB_CORE_ERROR("RigidBodyComponent.SetTranslation - Invalid entity!");
			return;
		}

		const auto& component = entity.getComponent<RigidBodyComponent>();
		if (component.runtimeBody == nullptr)
		{
			LAB_CORE_ERROR("RigidBodyComponent.SetTranslation - No b2Body exists for this entity!");
			return;
		}

		b2Body* body = (b2Body*)component.runtimeBody;
		body->SetTransform(b2Vec2(inTranslation->x, inTranslation->y), body->GetAngle());
	}

	float GlueFunctions::RigidBody_GetRotation(u64 entityID)
	{
		auto entity = GetEntity(entityID);
		if (!entity)
		{
			LAB_CORE_ERROR("RigidBodyComponent.GetRotation - Invalid entity!");
			return 0.0f;
		}

		const auto& component = entity.getComponent<RigidBodyComponent>();
		if (component.runtimeBody == nullptr)
		{
			LAB_CORE_ERROR("RigidBodyComponent.GetRotation - No b2Body exists for this entity!");
			return 0.0f;
		}

		return ((b2Body*)component.runtimeBody)->GetAngle();
	}

	void GlueFunctions::RigidBody_SetRotation(u64 entityID, float rotation)
	{
		auto entity = GetEntity(entityID);
		if (!entity)
		{
			LAB_CORE_ERROR("RigidBodyComponent.SetRotation - Invalid entity!");
			return;
		}

		const auto& component = entity.getComponent<RigidBodyComponent>();
		if (component.runtimeBody == nullptr)
		{
			LAB_CORE_ERROR("RigidBodyComponent.SetRotation - No b2Body exists for this entity!");
			return;
		}

		b2Body* body = (b2Body*)component.runtimeBody;
		body->SetTransform(body->GetPosition(), rotation);
	}

	float GlueFunctions::RigidBody_GetMass(u64 entityID)
	{
		auto entity = GetEntity(entityID);
		if (!entity)
		{
			LAB_CORE_ERROR("RigidBodyComponent.GetMass - Invalid entity!");
			return 0.0f;
		}

		const auto& component = entity.getComponent<RigidBodyComponent>();
		if (component.runtimeBody == nullptr)
		{
			LAB_CORE_ERROR("RigidBodyComponent.GetMass - No b2Body exists for this entity!");
			return 0.0f;
		}

		return ((b2Body*)component.runtimeBody)->GetMass();
	}

	void GlueFunctions::RigidBody_SetMass(u64 entityID, float mass)
	{
		auto entity = GetEntity(entityID);
		if (!entity)
		{
			LAB_CORE_ERROR("RigidBodyComponent.SetMass - Invalid entity!");
			return;
		}

		auto& component = entity.getComponent<RigidBodyComponent>();
		if (component.runtimeBody == nullptr)
		{
			LAB_CORE_ERROR("RigidBodyComponent.SetMass - No b2Body exists for this entity!");
			return;
		}

		b2Body* body = (b2Body*)component.runtimeBody;
		b2MassData massData = body->GetMassData();
		massData.mass = mass;
		body->SetMassData(&massData);

		component.mass = mass;
	}

	void GlueFunctions::RigidBody_GetLinearVelocity(u64 entityID, glm::vec2* outVelocity)
	{
		auto entity = GetEntity(entityID);
		if (!entity)
		{
			LAB_CORE_ERROR("RigidBodyComponent.GetLinearVelocity - Invalid entity!");
			return;
		}

		const auto& component = entity.getComponent<RigidBodyComponent>();
		if (component.runtimeBody == nullptr)
		{
			LAB_CORE_ERROR("RigidBodyComponent.GetLinearVelocity - No b2Body exists for this entity!");
			return;
		}

		b2Body* body = (b2Body*)component.runtimeBody;
		const b2Vec2& linearVelocity = body->GetLinearVelocity();
		outVelocity->x = linearVelocity.x;
		outVelocity->y = linearVelocity.y;
	}

	void GlueFunctions::RigidBody_SetLinearVelocity(u64 entityID, glm::vec2* inVelocity)
	{
		auto entity = GetEntity(entityID);
		if (!entity)
		{
			LAB_CORE_ERROR("RigidBodyComponent.SetLinearVelocity - Invalid entity!");
			return;
		}

		auto& component = entity.getComponent<RigidBodyComponent>();
		if (component.runtimeBody == nullptr)
		{
			LAB_CORE_ERROR("RigidBodyComponent.SetLinearVelocity - No b2Body exists for this entity!");
			return;
		}

		b2Body* body = (b2Body*)component.runtimeBody;
		body->SetLinearVelocity(b2Vec2(inVelocity->x, inVelocity->y));
	}

	float GlueFunctions::RigidBody_GetGravityScale(u64 entityID)
	{
		auto entity = GetEntity(entityID);
		if (!entity)
		{
			LAB_CORE_ERROR("RigidBodyComponent.GetGravityScale - Invalid entity!");
			return 0.0f;
		}

		const auto& component = entity.getComponent<RigidBodyComponent>();
		if (component.runtimeBody == nullptr)
		{
			LAB_CORE_ERROR("RigidBodyComponent.GetGravityScale - No b2Body exists for this entity!");
			return 0.0f;
		}

		return ((b2Body*)component.runtimeBody)->GetGravityScale();
	}

	void GlueFunctions::RigidBody_SetGravityScale(u64 entityID, float gravityScale)
	{
		auto entity = GetEntity(entityID);
		if (!entity)
		{
			LAB_CORE_ERROR("RigidBodyComponent.SetGravityScale - Invalid entity!");
			return;
		}

		auto& component = entity.getComponent<RigidBodyComponent>();
		if (component.runtimeBody == nullptr)
		{
			LAB_CORE_ERROR("RigidBodyComponent.SetGravityScale - No b2Body exists for this entity!");
			return;
		}

		b2Body* body = (b2Body*)component.runtimeBody;
		body->SetGravityScale(gravityScale);
		component.gravityScale = gravityScale;
	}

	void GlueFunctions::RigidBody_ApplyLinearImpulse(u64 entityID, glm::vec2* inImpulse, glm::vec2* inOffset, bool wake)
	{
		auto entity = GetEntity(entityID);
		if (!entity)
		{
			LAB_CORE_ERROR("RigidBodyComponent.ApplyLinearImpulse - Invalid entity!");
			return;
		}

		auto& component = entity.getComponent<RigidBodyComponent>();
		if (component.runtimeBody == nullptr)
		{
			LAB_CORE_ERROR("RigidBodyComponent.ApplyLinearImpulse - No b2Body exists for this entity!");
			return;
		}

		if (component.type != RigidBodyComponent::BodyType::Dynamic)
		{
			LAB_CORE_ERROR("RigidBodyComponent.ApplyLinearImpulse - Cannot add linear impulse to non-dynamic body!");
			return;
		}

		b2Body* body = (b2Body*)component.runtimeBody;
		body->ApplyLinearImpulse(*(const b2Vec2*)inImpulse, body->GetWorldCenter() + *(const b2Vec2*)inOffset, wake);
	}

	void GlueFunctions::RigidBody_ApplyAngularImpulse(u64 entityID, float impulse, bool wake)
	{
		auto entity = GetEntity(entityID);
		if (!entity)
		{
			LAB_CORE_ERROR("RigidBodyComponent.ApplyAngularImpulse - Invalid entity!");
			return;
		}

		auto& component = entity.getComponent<RigidBodyComponent>();
		if (component.runtimeBody == nullptr)
		{
			LAB_CORE_ERROR("RigidBodyComponent.ApplyAngularImpulse - No b2Body exists for this entity!");
			return;
		}

		if (component.type != RigidBodyComponent::BodyType::Dynamic)
		{
			LAB_CORE_ERROR("RigidBodyComponent.ApplyAngularImpulse - Cannot add angular impulse to non-dynamic body!");
			return;
		}

		b2Body* body = (b2Body*)component.runtimeBody;
		body->ApplyAngularImpulse(impulse, wake);
	}

	void GlueFunctions::RigidBody_AddForce(u64 entityID, glm::vec3* inForce, glm::vec3* inOffset, bool wake)
	{
		auto entity = GetEntity(entityID);
		if (!entity)
		{
			LAB_CORE_ERROR("RigidBodyComponent.AddForce - Invalid entity!");
			return;
		}

		auto& component = entity.getComponent<RigidBodyComponent>();
		if (component.runtimeBody == nullptr)
		{
			LAB_CORE_ERROR("RigidBodyComponent.AddForce - No b2Body exists for this entity!");
			return;
		}

		if (component.type != RigidBodyComponent::BodyType::Dynamic)
		{
			LAB_CORE_ERROR("RigidBodyComponent.AddForce - Cannot apply force to non-dynamic body!");
			return;
		}

		b2Body* body = (b2Body*)component.runtimeBody;
		body->ApplyForce(*(const b2Vec2*)inForce, body->GetWorldCenter() + *(const b2Vec2*)inOffset, wake);
	}

	void GlueFunctions::RigidBody_AddTorque(u64 entityID, float torque, bool wake)
	{
		auto entity = GetEntity(entityID);
		if (!entity)
		{
			LAB_CORE_ERROR("RigidBodyComponent.AddTorque - Invalid entity!");
			return;
		}

		auto& component = entity.getComponent<RigidBodyComponent>();
		if (component.runtimeBody == nullptr)
		{
			LAB_CORE_ERROR("RigidBodyComponent.AddTorque - No b2Body exists for this entity!");
			return;
		}

		if (component.type != RigidBodyComponent::BodyType::Dynamic)
		{
			LAB_CORE_ERROR("RigidBodyComponent.AddTorque - Cannot apply torque to non-dynamic body!");
			return;
		}

		b2Body* body = (b2Body*)component.runtimeBody;
		body->ApplyTorque(torque, wake);
	}

#pragma endregion

#pragma region AnimationComponent

	void GlueFunctions::Animation_PlayAnimation(u64 entityID, bool reset, bool playOnce)
	{
		Entity entity = GetEntity(entityID);
		if (!entity)
		{
			LAB_CORE_ERROR("Animation.PlayAnimation called on an invalid Entity!");
			return;
		}

		if (!entity.hasComponent<AnimationComponent>())
		{
			LAB_CORE_ERROR("Animation.PlayAnimation called on entity with no animation!");
			return;
		}

		auto& animationComp = entity.getComponent<AnimationComponent>();
		Ref<Animation> animation = AssetManager::GetAsset<Animation>(animationComp.handle);
		if (!animation)
		{
			LAB_CORE_ERROR("Animation.PlayAnimation could not retrieve Animation asset!");
			return;
		}

		if (reset)
			animation->reset();

		animationComp.playing = true;
		animationComp.playOnce = playOnce;
	}

	void GlueFunctions::Animation_PlayAnimation(u64 entityID, AssetHandle animationHandle, bool playOnce)
	{
		Entity entity = GetEntity(entityID);
		if (!entity)
		{
			LAB_CORE_ERROR("Animation.PlayAnimation called on an invalid Entity!");
			return;
		}

		if (!animationHandle)
		{
			LAB_CORE_ERROR("Animation.PlayAnimation called with no animation on entity with no animation!");
			return;
		}

		auto& animationComp = entity.getComponent<AnimationComponent>();
		animationComp.handle = animationHandle;
		Ref<Animation> animation = AssetManager::GetAsset<Animation>(animationHandle);
		if (!animation)
		{
			LAB_CORE_ERROR("Animation.PlayAnimation could not retrieve Animation asset!");
			return;
		}

		animation->reset();
		animationComp.playing = true;
		animationComp.playOnce = playOnce;
	}

	void GlueFunctions::Animation_StopAnimation(u64 entityID)
	{
		Entity entity = GetEntity(entityID);
		if (!entity)
		{
			LAB_CORE_ERROR("Animation.PlayAnimation called on an invalid Entity!");
			return;
		}

		if (!entity.hasComponent<AnimationComponent>())
		{
			LAB_CORE_ERROR("Animation.PlayAnimation tried to stop Entity without animation component!");
			return;
		}
		entity.getComponent<AnimationComponent>().playing = false;
	}

#pragma endregion
}