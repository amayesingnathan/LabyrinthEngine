#include <Labyrinth.h>
#include "Labyrinth/Core/EntryPoint.h"

#include "Platforms/OpenGL/OpenGLShader.h"

#include "imgui/imgui.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Sandbox2D.h"

class Sandbox : public Labyrinth::Application
{
public:
	Sandbox() 
	{
		//pushLayer(new Labyrinth::ExampleLayer());
		pushLayer(new Sandbox2D());
	}

	~Sandbox() 
	{
	}
};

Labyrinth::Application* Labyrinth::CreateApplication()
{
	return new Sandbox();
}