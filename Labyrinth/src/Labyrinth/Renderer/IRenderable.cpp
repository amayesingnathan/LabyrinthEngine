#include "Lpch.h"
#include "IRenderable.h"

#include <glad/glad.h>

namespace Laby {

	void IRenderable::bindTexture(u32 slot) const
	{
		glBindTextureUnit(slot, getTextureID());
	};
}