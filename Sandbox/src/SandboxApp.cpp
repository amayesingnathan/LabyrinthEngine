#include <Labyrinth.h>

class Sandbox : public Labyrinth::Application
{
public:
	Sandbox() 
	{

	}

	~Sandbox() 
	{
	}
};

Labyrinth::Application* Labyrinth::CreateApplication()
{
	return new Sandbox();
}