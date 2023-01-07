#include "Lpch.h"
#include "Renderer.h"

#include <glad/glad.h>

#include "Renderer2D.h"

namespace Laby {

	void OpenGLMessageCallback(
		unsigned source,
		unsigned type,
		unsigned id,
		unsigned severity,
		i32 length,
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

    void Renderer::Init()
    {
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

		glEnable(GL_LINE_SMOOTH);

		Renderer2D::Init();
    }

    void Renderer::Shutdown()
    {
		Renderer2D::Shutdown();
    }

    void Renderer::SetViewport(u32 x, u32 y, u32 w, u32 h)
    {
		glViewport(x, y, w, h);
    }

    void Renderer::SetClearColor(const glm::vec4& colour)
    {
		glClearColor(colour.r, colour.g, colour.b, colour.a);
    }

    void Renderer::Clear()
    {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

	void Renderer::SetLineWidth(f32 width)
	{
		glLineWidth(width);
	}

	void Renderer::DrawIndexed(const Ref<VertexArray>& vertexArray, u32 indexCount)
	{
		u32 count = indexCount ? indexCount : vertexArray->getIndexBuffer()->getCount();
		vertexArray->bind();
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
#if LAB_DEBUG
		vertexArray->unbind();
#endif
	}

	void Renderer::DrawLines(const Ref<VertexArray>& vertexArray, u32 vertexCount)
	{
		vertexArray->bind();
		glDrawArrays(GL_LINES, 0, vertexCount);
#if LAB_DEBUG
		vertexArray->unbind();
#endif
	}

	void Renderer::EnableDepth()
	{
		glEnable(GL_DEPTH_TEST);
	}

	void Renderer::DisableDepth()
	{
		glDisable(GL_DEPTH_TEST);
	}
}