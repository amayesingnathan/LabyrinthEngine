#include <Labyrinth.h>

class ExampleLayer : public Labyrinth::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{
	}

	void onUpdate() override
	{
	}

	void onEvent(Labyrinth::Event& event) override
	{
		LAB_TRACE("{0}", event);
	}
};

class Sandbox : public Labyrinth::Application
{
public:
	Sandbox() 
	{
		pushLayer(new ExampleLayer());
		pushOverlay(new Labyrinth::ImGuiLayer());
	}

	~Sandbox() 
	{
	}
};

Labyrinth::Application* Labyrinth::CreateApplication()
{
	return new Sandbox();
}