#include <Labyrinth.h>
#include "Labyrinth/Common/EntryPoint.h"

#include "Sandbox2D.h"

#include "Tangle.h"

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

Labyrinth::Application* Labyrinth::CreateApplication(ApplicationCommandLineArgs args)
{
	TMat22 a = { {2.0f, 0.0f}, {0.0f, 2.0f} };
	TVec2 b = { 2.5f, 1.25f };

	auto c = a * b;
	auto d = a * a;

	float bx = b.x();

	TVec<5> vec5 = { 1.f, 1.f, 1.f, 1.f, 1.f };

	float vec5z = vec5.z();

	return new Sandbox();
}