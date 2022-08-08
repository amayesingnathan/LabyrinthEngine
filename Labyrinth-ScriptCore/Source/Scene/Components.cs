using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Labyrinth
{
	public abstract class Component
	{
		public Entity Entity { get; internal set; }

	}
	public class TagComponent : Component
	{
		public string Tag
		{
			get => Internal.Tag_GetTag(Entity.ID);
			set => Internal.Tag_SetTag(Entity.ID, value);
		}
	}

	public class TransformComponent : Component
	{
		public Vector3 Translation
		{
			get
			{
				Internal.Transform_GetTranslation(Entity.ID, out Vector3 translation);
				return translation;
			}
			set => Internal.Transform_SetTranslation(Entity.ID, ref value);
		}
		public Vector3 Rotation
		{
			get
			{
				Internal.Transform_GetTranslation(Entity.ID, out Vector3 rotation);
				return rotation;
			}
			set => Internal.Transform_SetRotation(Entity.ID, ref value);
		}
		public Vector3 Scale
		{
			get
			{
				Internal.Transform_GetScale(Entity.ID, out Vector3 scale);
				return scale;
			}
			set => Internal.Transform_SetScale(Entity.ID, ref value);
		}
	}

	public class CameraComponent : Component
	{
		public float VerticalFOV
		{
			get => Internal.Camera_GetVerticalFOV(Entity.ID);
			set => Internal.Camera_SetVerticalFOV(Entity.ID, value);
		}
	}
	public class ScriptComponent : Component
	{
		public object Instance => Internal.Script_GetInstance(Entity.ID);
	}
	public class SpriteRendererComponent : Component
	{
		// TODO
	}


	public enum RigidBodyBodyType
	{
		None = -1, Static, Dynamic, Kinematic
	}

	public class RigidBodyComponent : Component
	{
		public RigidBodyBodyType BodyType
		{
			get => Internal.RigidBody_GetBodyType(Entity.ID);
			set => Internal.RigidBody_SetBodyType(Entity.ID, value);
		}

		public Vector2 Translation
		{
			get
			{
				Internal.RigidBody_GetTranslation(Entity.ID, out Vector2 translation);
				return translation;
			}

			set => Internal.RigidBody_SetTranslation(Entity.ID, ref value);
		}

		public float Rotation
		{
			get => Internal.RigidBody_GetRotation(Entity.ID);
			set => Internal.RigidBody_SetRotation(Entity.ID, value);
		}

		public float Mass
		{
			get => Internal.RigidBody_GetMass(Entity.ID);
			set => Internal.RigidBody_SetMass(Entity.ID, value);
		}

		public Vector2 LinearVelocity
		{
			get
			{
				Internal.RigidBody_GetLinearVelocity(Entity.ID, out Vector2 velocity);
				return velocity;
			}

			set => Internal.RigidBody_SetLinearVelocity(Entity.ID, ref value);
		}

		public float GravityScale
		{
			get => Internal.RigidBody_GetGravityScale(Entity.ID);
			set => Internal.RigidBody_SetGravityScale(Entity.ID, value);
		}

		public void ApplyLinearImpulse(Vector2 impulse, Vector2 offset, bool wake)
			=> Internal.RigidBody_ApplyLinearImpulse(Entity.ID, ref impulse, ref offset, wake);

		public void ApplyAngularImpulse(float impulse, bool wake)
			=> Internal.RigidBody_ApplyAngularImpulse(Entity.ID, impulse, wake);

		public void AddForce(Vector2 force, Vector2 offset, bool wake)
			=> Internal.RigidBody_AddForce(Entity.ID, ref force, ref offset, wake);

		public void AddTorque(float torque, bool wake)
			=> Internal.RigidBody_AddTorque(Entity.ID, torque, wake);
	}

	public class BoxColliderComponent : Component
	{
	}

	public class CircleColliderComponent : Component
	{
	}
}
