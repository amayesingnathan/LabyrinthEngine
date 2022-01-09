#include "Lpch.h"
#include "RenderCommand.h"

#include "Platforms/OpenGL/OpenGLRendererAPI.h"

namespace Labyrinth {

	Single<RendererAPI> RenderCommand::sRendererAPI = CreateSingle<OpenGLRendererAPI>();

}