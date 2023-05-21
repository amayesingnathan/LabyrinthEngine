using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Labyrinth
{
	public static class Scene
	{
		// This entire Scene is currently static, since only one scene is active at a time.
		public static ulong PreLoad(string path) => Internal.Scene_PreLoadScene(path);
        public static void Load() => Internal.Scene_LoadPreLoadedScene();
        public static void Load(ulong sceneID) => Internal.Scene_LoadSceneFromID(sceneID);
        public static ulong Load(string path) => Internal.Scene_LoadSceneFromPath(path);
        public static ulong LoadClone(ulong sceneID) => Internal.Scene_LoadClonedScene(sceneID);
        public static void Unload(ulong sceneID) => Internal.Scene_UnloadScene(sceneID);

        public static Entity CreateEntity(string tag = "Unnamed") => new Entity(Internal.Scene_CreateEntity(tag));
		public static void DestroyEntity(Entity entity)
		{
			if (entity == null)
				return;

			if (!Internal.Scene_IsEntityValid(entity.ID))
				return;

			int hashCode = entity.ID.GetHashCode();

			// Remove this entity from the cache if it's been indexed either with the ID or Tag hash code
			if (!sEntityCache.Remove(hashCode))
				sEntityCache.Remove(entity.Tag.GetHashCode());

			Internal.Scene_DestroyEntity(entity.ID);
		}

		private static Dictionary<int, Entity> sEntityCache = new Dictionary<int, Entity>(50);

		public static Entity FindEntityByTag(string tag)
		{
			int hashCode = tag.GetHashCode();

			if (sEntityCache.ContainsKey(hashCode) && sEntityCache[hashCode] != null)
			{
				var entity = sEntityCache[hashCode];

				if (!Internal.Scene_IsEntityValid(entity.ID))
				{
					if (sEntityCache.ContainsKey(hashCode))
						sEntityCache.Remove(hashCode);
					entity = null;
				}

				return entity;
			}

			ulong entityID = Internal.Scene_FindEntityByTag(tag);
			Entity newEntity = entityID != 0 ? new Entity(entityID) : null;
			sEntityCache[hashCode] = newEntity;

			return newEntity;
		}

		public static Entity FindEntityByID(ulong entityID)
		{
			int hashCode = entityID.GetHashCode();
			if (sEntityCache.ContainsKey(hashCode) && sEntityCache[hashCode] != null)
			{
				var entity = sEntityCache[hashCode];

				if (!Internal.Scene_IsEntityValid(entity.ID))
				{
					if (sEntityCache.ContainsKey(hashCode))
						sEntityCache.Remove(hashCode);
					entity = null;
				}

				return entity;
			}

			if (!Internal.Scene_IsEntityValid(entityID))
				return null;

			Entity newEntity = new Entity(entityID);
			sEntityCache[hashCode] = newEntity;
			return newEntity;
		}

		public static Entity[] GetEntities() => Internal.Scene_GetEntities();
	}
}
