using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.CompilerServices;

namespace Labyrinth
{
    internal static class Internal
    {
        #region Logging
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Log_Trace(string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Log_Info(string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Log_Warning(string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Log_Error(string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Log_Critical(string text);
        #endregion

        #region Input
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_IsKeyPressed(KeyCode key);
        #endregion

        #region AssetHandle
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool AssetHandle_IsValid(ref AssetHandle handle);
        #endregion

        #region Scene 

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ulong Scene_PreLoadScene(string path);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Scene_LoadPreLoadedScene();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Scene_LoadSceneFromID(ulong sceneID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ulong Scene_LoadSceneFromPath(string path);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ulong Scene_LoadClonedScene(ulong sceneID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Scene_UnloadScene(ulong sceneID);


        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ulong Scene_FindEntityByTag(string tag);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Scene_IsEntityValid(ulong id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ulong Scene_CreateEntity(string tag);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Scene_DestroyEntity(ulong id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static Entity[] Scene_GetEntities();
        #endregion

        #region Entity

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ulong Entity_GetParent(ulong id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ulong Entity_SetParent(ulong entity, ulong parent);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static Entity[] Entity_GetChildren(ulong entity);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Entity_CreateComponent(ulong entity, Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool Entity_HasComponent(ulong entity, Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool Entity_RemoveComponent(ulong entity, Type type);

        #endregion

        #region TransformComponent

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Transform_GetTransform(ulong entUUID, out TransformComponent translation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Transform_SetTransform(ulong entUUID, ref TransformComponent translation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Transform_GetTranslation(ulong entUUID, out Vector3 translation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Transform_SetTranslation(ulong entUUID, ref Vector3 translation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Transform_GetRotation(ulong entUUID, out Vector3 rotation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Transform_SetRotation(ulong entUUID, ref Vector3 rotation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Transform_GetScale(ulong entUUID, out Vector3 scale);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Transform_SetScale(ulong entUUID, ref Vector3 scale);
        #endregion

        #region TagComponent

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static string Tag_GetTag(ulong entUUID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Tag_SetTag(ulong entUUID, string tag);

        #endregion

        #region CameraComponent

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float Camera_GetVerticalFOV(ulong entUUID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Camera_SetVerticalFOV(ulong entUUID, float fov);

        #endregion

        #region ScriptComponent

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern object Script_GetInstance(ulong entityID);

        #endregion

        #region RigidBodyComponent

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static RigidBodyBodyType RigidBody_GetBodyType(ulong entUUID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidBody_SetBodyType(ulong entUUID, RigidBodyBodyType type);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidBody_GetTranslation(ulong entUUID, out Vector2 translation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidBody_SetTranslation(ulong entUUID, ref Vector2 translation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float RigidBody_GetRotation(ulong entUUID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidBody_SetRotation(ulong entUUID, float rotation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float RigidBody_GetMass(ulong entUUID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidBody_SetMass(ulong entUUID, float mass);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidBody_GetLinearVelocity(ulong entUUID, out Vector2 linVelocity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidBody_SetLinearVelocity(ulong entUUID, ref Vector2 linVelocity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float RigidBody_GetGravityScale(ulong entUUID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidBody_SetGravityScale(ulong entUUID, float gravityScale);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float RigidBody_ApplyLinearImpulse(ulong entUUID, ref Vector2 inImpulse, ref Vector2 inOffset, bool wake);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float RigidBody_ApplyAngularImpulse(ulong entUUID, float impulse, bool wake);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float RigidBody_AddForce(ulong entUUID, ref Vector2 inForce, ref Vector2 inOffset, bool wake);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float RigidBody_AddTorque(ulong entUUID, float torque, bool wake);

        #endregion

        #region AnimationComponent

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Animation_PlayAnimation(ulong entUUID, bool reset, bool playOnce);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Animation_PlayAnimation(ulong entUUID, ref AssetHandle handle, bool playOnce);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Animation_StopAnimation(ulong entUUID);


        #endregion
    }
}
