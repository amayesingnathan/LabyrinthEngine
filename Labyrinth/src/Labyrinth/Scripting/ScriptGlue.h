#pragma once

#include "ScriptDefinitions.h"

#include <Labyrinth/IO/Input.h>
#include <Labyrinth/Scene/Components.h>

namespace Laby {

	class ScriptGlue
	{
	public:
		static void Register();

	private:
		static void RegisterComponentTypes();
		static void RegisterInternalCalls();
	};

	class GlueFunctions
	{
	private:
#pragma region Logging
		static void Log_Trace(MonoString* string);
		static void Log_Info(MonoString* string);
		static void Log_Warning(MonoString* string);
		static void Log_Error(MonoString* string);
		static void Log_Critical(MonoString* string);
#pragma endregion

#pragma region Input

		static bool Input_IsKeyPressed(KeyCode key);

#pragma endregion

#pragma region AssetHandle

		static bool AssetHandle_IsValid(AssetHandle assetHandle);

#pragma endregion

#pragma region Scene

		static u64 Scene_FindEntityByTag(MonoString* tag);
		static bool Scene_IsEntityValid(UUID entityID);
		static u64 Scene_CreateEntity(MonoString* tag);
		static void Scene_DestroyEntity(UUID entityID);
		static MonoArray* Scene_GetEntities();

#pragma endregion

#pragma region Entity

		static UUID Entity_GetParent(UUID entityID);
		static void Entity_SetParent(UUID entityID, UUID parentID);
		static MonoArray* Entity_GetChildren(uint64_t entityID);
		static void Entity_CreateComponent(uint64_t entityID, MonoReflectionType* reflectionType);
		static bool Entity_HasComponent(uint64_t entityID, MonoReflectionType* reflectionType);
		static bool Entity_RemoveComponent(uint64_t entityID, MonoReflectionType* componentType);

#pragma endregion

#pragma region TagComponent

		static MonoString* Tag_GetTag(UUID id);
		static void Tag_SetTag(UUID id, MonoString* inTag);

#pragma endregion

#pragma region TransformComponent

		static void Transform_GetTransform(UUID id, TransformComponent* outTranslation);
		static void Transform_SetTransform(UUID id, TransformComponent* translation);
		static void Transform_GetTranslation(UUID id, glm::vec3* outTranslation);
		static void Transform_SetTranslation(UUID id, glm::vec3* translation);
		static void Transform_GetRotation(UUID id, glm::vec3* rotation);
		static void Transform_SetRotation(UUID id, glm::vec3* rotation);
		static void Transform_GetScale(UUID id, glm::vec3* outScale);
		static void Transform_SetScale(UUID id, glm::vec3* scale);

#pragma endregion

#pragma region CameraComponent

		static float Camera_GetVerticalFOV(UUID entityID);
		static void Camera_SetVerticalFOV(UUID entityID, float verticalFOV);

#pragma endregion

#pragma region ScriptComponent

		static MonoObject* Script_GetInstance(u64 entityID);

#pragma endregion

#pragma region RigidBodyComponent

		static RigidBodyComponent::BodyType RigidBody_GetBodyType(u64 entityID);
		static void RigidBody_SetBodyType(u64 entityID, RigidBodyComponent::BodyType inType);
		static void RigidBody_GetTranslation(u64 entityID, glm::vec2* outTranslation);
		static void RigidBody_SetTranslation(u64 entityID, glm::vec2* inTranslation);
		static float RigidBody_GetRotation(u64 entityID);
		static void RigidBody_SetRotation(u64 entityID, float rotation);
		static float RigidBody_GetMass(u64 entityID);
		static void RigidBody_SetMass(u64 entityID, float mass);
		static void RigidBody_GetLinearVelocity(u64 entityID, glm::vec2* outVelocity);
		static void RigidBody_SetLinearVelocity(u64 entityID, glm::vec2* inVelocity);
		static float RigidBody_GetGravityScale(u64 entityID);
		static void RigidBody_SetGravityScale(u64 entityID, float gravityScale);
		static void RigidBody_ApplyLinearImpulse(u64 entityID, glm::vec2* inImpulse, glm::vec2* inOffset, bool wake);
		static void RigidBody_ApplyAngularImpulse(u64 entityID, float impulse, bool wake);
		static void RigidBody_AddForce(u64 entityID, glm::vec3* inForce, glm::vec3* inOffset, bool wake);
		static void RigidBody_AddTorque(u64 entityID, float torque, bool wake);

#pragma endregion

#pragma region AnimationComponent

		static void Animation_PlayAnimation(u64 entityID, bool reset = true, bool playOnce = false);
		static void Animation_PlayAnimation(u64 entityID, AssetHandle animationHandle, bool playOnce = false);
		static void Animation_StopAnimation(u64 entityID);

#pragma endregion

		friend class ScriptGlue;
	};

}