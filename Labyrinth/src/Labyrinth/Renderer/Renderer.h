#pragma once

#include "RenderCommand.h"

#include "Shader.h"
#include "Camera.h"

namespace Labyrinth {

	class Renderer
	{
	public:
		static void BeginState(OrthographicCamera& camera);
		static void EndState();

		static void Send(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray);

		static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static SceneData* sSceneData;
	};

}