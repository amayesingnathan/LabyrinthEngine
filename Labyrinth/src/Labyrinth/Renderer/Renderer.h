#pragma once

namespace Labyrinth {

	enum class RendererAPI
	{
		None	= 0,
		OpenGL	= 1
	};

	class Renderer
	{
	public:
		inline static RendererAPI GetAPI() { return sRendererAPI; }
		inline static void SetAPI(RendererAPI newAPI) { sRendererAPI = newAPI; }

	private:
		static RendererAPI sRendererAPI;
	};

}