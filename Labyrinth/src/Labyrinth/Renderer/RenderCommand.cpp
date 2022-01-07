#include "Lpch.h"
#include "RenderCommand.h"

#include "Platforms/OpenGL/OpenGLRendererAPI.h"

namespace Labyrinth {

	RendererAPI* RenderCommand::sRendererAPI = new OpenGLRendererAPI;

}