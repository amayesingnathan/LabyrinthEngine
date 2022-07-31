#include "Lpch.h"
#include "ScriptGlue.h"

#include "ScriptEngine.h"
#include "MarshalUtils.h"

#include <Labyrinth/Assets/AssetManager.h>

#include <Labyrinth/Core/Buffer.h>

#include <Labyrinth/IO/Input.h>

#include <Labyrinth/Scene/Scene.h>
#include <Labyrinth/Scene/Entity.h>
#include <Labyrinth/Scene/Components.h>

#include <mono/metadata/class.h>
#include <mono/metadata/object.h>
#include <mono/metadata/reflection.h>
#include <mono/metadata/loader.h>
#include <mono/jit/jit.h>

#include <glm/glm.hpp>

#include <box2d/box2d.h>

namespace Labyrinth {

	static inline Entity GetEntity(UUID entityID)
	{
		Ref<Scene> scene = ScriptEngine::GetContext();
		LAB_CORE_ASSERT(scene, "No active scene!");
		return scene->findEntity(entityID);
	};

	static std::unordered_map<MonoType*, std::function<void(Entity&)>> sCreateComponentFuncs;
	static std::unordered_map<MonoType*, std::function<bool(Entity&)>> sHasComponentFuncs;
	static std::unordered_map<MonoType*, std::function<void(Entity&)>> sRemoveComponentFuncs;

#define LAB_ADD_INTERNAL_CALL(funcName) mono_add_internal_call("Labyrinth.Internal::"#funcName, funcName)

#define LAB_REGISTER_COMPONENT(Type) \
		{\
			MonoType* managedType = mono_reflection_type_from_name("Labyrinth." #Type, ScriptEngine::GetData()->coreAssemblyImage);\
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

#pragma region Logging

	static void Log_Trace(MonoString* string)
	{
		char* cStr = mono_string_to_utf8(string);
		LAB_CORE_TRACE(cStr);
		mono_free(cStr);
	}
	static void Log_Info(MonoString* string)
	{
		char* cStr = mono_string_to_utf8(string);
		LAB_CORE_INFO(cStr);
		mono_free(cStr);
	}
	static void Log_Warning(MonoString* string)
	{
		char* cStr = mono_string_to_utf8(string);
		LAB_CORE_WARN(cStr);
		mono_free(cStr);
	}
	static void Log_Error(MonoString* string)
	{
		char* cStr = mono_string_to_utf8(string);
		LAB_CORE_ERROR(cStr);
		mono_free(cStr);
	}
	static void Log_Critical(MonoString* string)
	{
		char* cStr = mono_string_to_utf8(string);
		LAB_CORE_CRITICAL(cStr);
		mono_free(cStr);
	}
#pragma endregion

#pragma region Input
	static bool Input_IsKeyPressed(KeyCode key)
	{
		return Input::IsKeyPressed(key);
	}
#pragma endregion

#pragma region AssetHandle

	static bool AssetHandle_IsValid(AssetHandle assetHandle)
	{
		return AssetManager::IsAssetHandleValid(assetHandle);
	}

#pragma endregion

#pragma region Scene

	static bool Scene_IsEntityValid(UUID entityID)
	{
		if (entityID == 0)
			return false;

		return (bool)(ScriptEngine::GetContext()->findEntity(entityID));
	}

	static u64 Scene_CreateEntity(MonoString* tag)
	{
		Ref<Scene> scene = ScriptEngine::GetContext();
		LAB_CORE_ASSERT(scene, "No active scene!");
		return scene->CreateEntity(MarshalUtils::MonoStringToUTF8(tag)).getUUID();
	}

	static void Scene_DestroyEntity(UUID entityID)
	{
		Entity entity = GetEntity(entityID);
		if (!entity)
			return;

		Ref<Scene> scene = ScriptEngine::GetContext();
		scene->DestroyEntity(entity);
	}

#pragma endregion

#pragma region Entity

	static UUID Entity_GetParent(UUID entityID)
	{
		auto entity = GetEntity(entityID);
		if (!entity)
			return 0;
		return entity.getParent();
	}

	static void Entity_SetParent(UUID entityID, UUID parentID)
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

	static MonoArray* Entity_GetChildren(uint64_t entityID)
	{
		Entity entity = GetEntity(entityID);
		if (!entity)
		{
			LAB_CORE_ERROR("Entity.GetChildren - Invalid entity!");
			return nullptr;
		}

		const auto& children = entity.getChildren();
		ScriptEngineData* scriptData = ScriptEngine::GetData();
		MonoArray* result = mono_array_new(scriptData->appDomain, *scriptData->entityClass, children.size());
		for (uint32_t i = 0; i < children.size(); i++)
		{
			MonoObject* boxed = scriptData->entityClass->instantiate(children[i]);
			mono_array_setref(result, i, boxed);
		}

		return result;
	}

	static void Entity_CreateComponent(uint64_t entityID, MonoReflectionType* reflectionType)
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

	static bool Entity_HasComponent(uint64_t entityID, MonoReflectionType* reflectionType)
	{
		LAB_PROFILE_FUNCTION();

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

	static bool Entity_RemoveComponent(uint64_t entityID, MonoReflectionType* componentType)
	{
		LAB_PROFILE_FUNCTION();

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
	static MonoString* Tag_GetTag(UUID id)
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
	static void Tag_SetTag(UUID id, MonoString* inTag)
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
	static void Transform_GetTransform(UUID id, TransformComponent* outTranslation)
	{
		auto entity = GetEntity(id);
		if (!entity)
		{
			LAB_CORE_ERROR("TransformComponent.GetTransform - Invalid entity!");
			return;
		}

		*outTranslation = entity.getComponent<TransformComponent>();
	}
	static void Transform_SetTransform(UUID id, TransformComponent* translation)
	{
		auto entity = GetEntity(id);
		if (!entity)
		{
			LAB_CORE_ERROR("TransformComponent.SetTransform - Invalid entity!");
			return;
		}

		entity.getComponent<TransformComponent>() = *translation;
	}
	static void Transform_GetTranslation(UUID id, glm::vec3* outTranslation)
	{
		auto entity = GetEntity(id);
		if (!entity)
		{
			LAB_CORE_ERROR("TransformComponent.GetTranslation - Invalid entity!");
			return;
		}

		*outTranslation = entity.getComponent<TransformComponent>().translation;
	}
	static void Transform_SetTranslation(UUID id, glm::vec3* translation)
	{
		auto entity = GetEntity(id);
		if (!entity)
		{
			LAB_CORE_ERROR("TransformComponent.SetTranslation - Invalid entity!");
			return;
		}

		entity.getComponent<TransformComponent>().translation = *translation;
	}
	static void Transform_GetRotation(UUID id, glm::vec3* rotation)
	{
		auto entity = GetEntity(id);
		if (!entity)
		{
			LAB_CORE_ERROR("TransformComponent.GetRotation- Invalid entity!");
			return;
		}

		*rotation = entity.getComponent<TransformComponent>().rotation;
	}
	static void Transform_SetRotation(UUID id, glm::vec3* rotation)
	{
		auto entity = GetEntity(id);
		if (!entity)
		{
			LAB_CORE_ERROR("TransformComponent.SetRotation - Invalid entity!");
			return;
		}

		entity.getComponent<TransformComponent>().rotation = *rotation;
	}
	static void Transform_GetScale(UUID id, glm::vec3* outScale)
	{
		auto entity = GetEntity(id);
		if (!entity)
		{
			LAB_CORE_ERROR("TransformComponent.GetScale- Invalid entity!");
			return;
		}

		*outScale = entity.getComponent<TransformComponent>().scale;
	}
	static void Transform_SetScale(UUID id, glm::vec3* scale)
	{
		auto entity = GetEntity(id);
		if (!entity)
		{
			LAB_CORE_ERROR("TransformComponent.SetScale- Invalid entity!");
			return;
		}

		entity.getComponent<TransformComponent>().scale = *scale;
	}
#pragma endregion

#pragma region CameraComponent

	static float Camera_GetVerticalFOV(UUID entityID)
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

	static void Camera_SetVerticalFOV(UUID entityID, float verticalFOV)
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

#pragma region RigidBodyComponent

	static RigidBodyComponent::BodyType RigidBody_GetBodyType(u64 entityID)
	{
		auto entity = GetEntity(entityID);
		if (!entity)
		{
			LAB_CORE_ERROR("RigidBodyComponent.GetBodyType - Invalid entity!");
			return RigidBodyComponent::BodyType::None;
		}

		return entity.getComponent<RigidBodyComponent>().type;
	}

	static void RigidBody_SetBodyType(u64 entityID, RigidBodyComponent::BodyType inType)
	{
		auto entity = GetEntity(entityID);
		if (!entity)
		{
			LAB_CORE_ERROR("RigidBodyComponent.SetBodyType - Invalid entity!");
			return;
		}

		entity.getComponent<RigidBodyComponent>().type = inType;
	}

	static void RigidBody_GetTranslation(u64 entityID, glm::vec2* outTranslation)
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

	static void RigidBody_SetTranslation(u64 entityID, glm::vec2* inTranslation)
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

	static float RigidBody_GetRotation(u64 entityID)
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

	static void RigidBody_SetRotation(u64 entityID, float rotation)
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

	static float RigidBody_GetMass(u64 entityID)
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

	static void RigidBody_SetMass(u64 entityID, float mass)
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

	static void RigidBody_GetLinearVelocity(u64 entityID, glm::vec2* outVelocity)
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

	static void RigidBody_SetLinearVelocity(u64 entityID, glm::vec2* inVelocity)
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

	static float RigidBody_GetGravityScale(u64 entityID)
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

	static void RigidBody_SetGravityScale(u64 entityID, float gravityScale)
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

	static void RigidBody_ApplyLinearImpulse(u64 entityID, glm::vec2* inImpulse, glm::vec2* inOffset, bool wake)
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

	static void RigidBody_ApplyAngularImpulse(u64 entityID, float impulse, bool wake)
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

	static void RigidBody_AddForce(u64 entityID, glm::vec3* inForce, glm::vec3* inOffset, bool wake)
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

	static void RigidBody_AddTorque(u64 entityID, float torque, bool wake)
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

		LAB_ADD_INTERNAL_CALL(Scene_IsEntityValid);
		LAB_ADD_INTERNAL_CALL(Scene_CreateEntity);
		LAB_ADD_INTERNAL_CALL(Scene_DestroyEntity);

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

}