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

        protected Entity() { ID = 0; }
        internal Entity(ulong id) { ID = id; }

        public Vector3 Translation
        {
            get
            {
                Internal.Entity_GetTranslation(ID, out Vector3 translation);
                return translation;
            }
            set
            {
                Internal.Entity_SetTranslation(ID, ref value);
            }
        }

        public virtual void OnCreate() { }
        public virtual void OnUpdate(float ts) { }
    }

}
