#pragma once

#include <glm/glm.hpp>

namespace Labyrinth {

	class Shader
	{
	public:
		Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
		~Shader();

		void bind() const;
		void unbind() const;

		void uploadUniformMat4(const std::string& name, const glm::mat4& matrix);

	private:
		uint32_t mRendererID;
	};

}