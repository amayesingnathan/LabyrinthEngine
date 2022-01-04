#include <Labyrinth.h>

#include "imgui/imgui.h"

class ExampleLayer : public Labyrinth::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{
	}

	virtual void onImGuiRender() override
	{
		ImGui::Begin("Test");
		ImGui::Text("Hello World");
		ImGui::End();
	}

	void onUpdate() override
	{
	}

	void onEvent(Labyrinth::Event& event) override
	{
		if (event.getEventType() == Labyrinth::EventType::KeyPressed)
		{
			Labyrinth::KeyPressedEvent& e = (Labyrinth::KeyPressedEvent&)event;
			if (e.getKeyCode() == LAB_KEY_TAB)
				LAB_TRACE("Tab key is pressed (event)!");

			LAB_TRACE("{0}", e.getKeyName());
		}
	}
};

class Sandbox : public Labyrinth::Application
{
public:
	Sandbox() 
	{
		pushLayer(new ExampleLayer());
	}

	~Sandbox() 
	{
	}
};

Labyrinth::Application* Labyrinth::CreateApplication()
{
	return new Sandbox();
}