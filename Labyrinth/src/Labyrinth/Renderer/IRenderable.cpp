#include "Lpch.h"
#include "IRenderable.h"

#include <glad/glad.h>

namespace Laby {

	void IRenderable::BindTexture(u32 slot) const
	{
		glBindTextureUnit(slot, GetTextureID());
	};
}