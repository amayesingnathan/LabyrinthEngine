using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Labyrinth
{
	public static class Scene
	{
		// This entire Scene is currently static, since we can only have one scene at a time right now

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
