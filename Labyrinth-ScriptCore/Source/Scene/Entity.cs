using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Labyrinth
{
    public class Entity
    {
        public readonly ulong ID;
		private Entity mParent;
		private Dictionary<Type, Component> mComponentCache = new Dictionary<Type, Component>();

		protected Entity() { ID = 0; }
        internal Entity(ulong id) { ID = id; }
		public string Tag => GetComponent<TagComponent>().Tag;

		public Vector3 Translation
		{
			get => GetComponent<TransformComponent>().Translation;
			set => GetComponent<TransformComponent>().Translation = value;
		}

		public Vector3 Rotation
		{
			get => GetComponent<TransformComponent>().Rotation;
			set => GetComponent<TransformComponent>().Rotation = value;
		}

		public Vector3 Scale
		{
			get => GetComponent<TransformComponent>().Scale;
			set => GetComponent<TransformComponent>().Scale = value;
		}

		public Entity Parent
		{
			get
			{
				ulong parentID = Internal.Entity_GetParent(ID);
				if (mParent == null || mParent.ID != parentID)
				{
					mParent = Internal.Scene_IsEntityValid(parentID)
						? new Entity(parentID) : null;
				}

				return mParent;
			}
			set => Internal.Entity_SetParent(ID, value.ID);
		}

		public Entity[] Children => Internal.Entity_GetChildren(ID);

		public virtual void OnCreate() { }
		public virtual void OnUpdate(float ts) { }

		public bool HasComponent<T>() where T : Component => Internal.Entity_HasComponent(ID, typeof(T));
        public bool HasComponent(Type type) => Internal.Entity_HasComponent(ID, type);
		public T GetComponent<T>() where T : Component, new()
		{
			Type componentType = typeof(T);

			if (!HasComponent<T>())
			{
				if (mComponentCache.ContainsKey(componentType))
					mComponentCache.Remove(componentType);

				return null;
			}

			if (!mComponentCache.ContainsKey(componentType))
			{
				T component = new T { Entity = this };
				mComponentCache.Add(componentType, component);
				return component;
			}

			return mComponentCache[componentType] as T;
		}
		public bool RemoveComponent<T>() where T : Component
		{
			Type componentType = typeof(T);
			bool removed = Internal.Entity_RemoveComponent(ID, componentType);

			if (removed && mComponentCache.ContainsKey(componentType))
				mComponentCache.Remove(componentType);

			return removed;
		}
    }

}
