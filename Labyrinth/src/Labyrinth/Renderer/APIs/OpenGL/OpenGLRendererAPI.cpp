#include "Lpch.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Labyrinth {

	void OpenGLMessageCallback(
		unsigned source,
		unsigned type,
		unsigned id,
		unsigned severity,
		int length,
		const char* message,
		const void* userParam)
	{
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:         LAB_CORE_CRITICAL(message); return;
		case GL_DEBUG_SEVERITY_MEDIUM:       LAB_CORE_ERROR(message); return;
		case GL_DEBUG_SEVERITY_LOW:          LAB_CORE_WARN(message); return;
		case GL_DEBUG_SEVERITY_NOTIFICATION: LAB_CORE_TRACE(message); return;
		}

		LAB_CORE_ASSERT(false, "Unknown severity level!");
	}

	void OpenGLRendererAPI::init()
	{
		LAB_PROFILE_FUNCTION();

#ifdef LAB_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGLMessageCallback, nullptr);

		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);

		glDepthFunc(GL_LESS);
		glEnable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::setViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
	{
		glViewport(x, y, w, h);
	}

	void OpenGLRendererAPI::setClearColor(const glm::vec4& colour)
	{
		glClearColor(colour.r, colour.g, colour.b, colour.a);
	}

	void OpenGLRendererAPI::clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::drawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		uint32_t count = indexCount ? indexCount : vertexArray->getIndexBuffer()->getCount();
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLRendererAPI::enableDepth()
	{
		glEnable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::disableDepth()
	{
		glDisable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::enableBlend()
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
	}

	void OpenGLRendererAPI::disableBlend()
	{
		glDisable(GL_BLEND);
	}
}