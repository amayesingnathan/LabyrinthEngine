#pragma once

#include "RenderCommand.h"

#include "Shader.h"
#include "Camera.h"

namespace Labyrinth {

	class Renderer
	{
	public:
		static void Init();
		static void OnWindowResize(uint32_t w, uint32_t h);

		static void BeginState(OrthographicCamera& camera);
		static void EndState();

		static void Send(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));
		
		static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static SceneData* sSceneData;
	};

}