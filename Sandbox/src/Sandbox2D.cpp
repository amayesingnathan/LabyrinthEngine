#include "Sandbox2D.h"
#include <Labyrinth.h>
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "lua.hpp"

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), mCameraController(1280.0f / 720.0f, true)
{}

void Sandbox2D::onAttach()
{
	LAB_PROFILE_FUNCTION();

	struct TestComponent
	{
		float x;
		float y;

		void move(float newX, float newY)
		{
			x += newX;
			y += newY;
		}
	};
		
	auto CreateSprite = [](lua_State* L) -> int
	{
		TestComponent* comp = Labyrinth::Cast<TestComponent>(lua_newuserdata(L, sizeof(TestComponent)));
		comp->x = 0.0f;
		comp->y = 0.0f;
		return 1;
	};

	auto MoveSprite = [](lua_State* L) -> int
	{
		TestComponent* comp = Labyrinth::Cast<TestComponent>(lua_touserdata(L, 1));
		float velX = (float)lua_tonumber(L, -2);
		float velY = (float)lua_tonumber(L, -1);
		comp->move(velX, velY);
		return 1;
	};

	lua_State* L = luaL_newstate();
	lua_pushcfunction(L, CreateSprite);
	lua_setglobal(L, "CreateSprite");
	lua_pushcfunction(L, MoveSprite);
	lua_setglobal(L, "MoveSprite");
	luaL_dofile(L, "assets/scripts/example.lua");
	lua_getglobal(L, "sprite");
	if (lua_isuserdata(L, -1))
	{
		TestComponent* comp = Labyrinth::Cast<TestComponent>(lua_touserdata(L, -1));
		LAB_TRACE("X: {0}; Y: {1}", comp->x, comp->y);
	}
	lua_close(L);

	mCheckerboardTexture = Labyrinth::Texture2D::Create("assets/textures/checkerboard.png");

	mSpriteSheet = Labyrinth::Texture2DSheet::CreateFromPath("assets/textures/minecraft.png", { 64.0f, 64.0f });
	//mTNT = Labyrinth::SubTexture2D::CreateFromCoords(mSpriteSheet, { 8.0f, 0.0f }, { 1.0f, 1.0f });
	mSpriteSheet->createSubTex("TNT", { 8.0f, 0.0f }, {1.0f, 1.0f});
}

void Sandbox2D::onDetach()
{
	LAB_PROFILE_FUNCTION();
}

void Sandbox2D::onUpdate(Labyrinth::Timestep ts)
{
	LAB_PROFILE_FUNCTION();

	mCameraController.onUpdate(ts);
			
	{
		LAB_PROFILE_SCOPE("Renderer Prep");
		Labyrinth::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Labyrinth::RenderCommand::Clear();
	}

	{
		static float rotation = 0.0f;
		rotation += ts * 50;

		LAB_PROFILE_SCOPE("Renderer Draw");
		Labyrinth::Renderer2D::BeginState(mCameraController.getCamera());
		
		//Labyrinth::Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 10.0f, 10.0f }, mSpriteSheet->getTex());
		Labyrinth::Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 2.0f, 2.0f }, mSpriteSheet->getSubTex("TNT"));
		//Labyrinth::Renderer2D::DrawQuad({ 2.0f, 2.0f }, { 2.0f, 2.0f }, Labyrinth::SubTexture2D::CreateFromCoords(mSpriteSheet, { 13.0f, 3.0f }, { 1.0f, 1.0f }));
		//Labyrinth::Renderer2D::DrawRotatedQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, -rotation, mSquareColor2);
		//Labyrinth::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.5f }, mSquareColor);
		//Labyrinth::Renderer2D::DrawQuad({ -0.5f, -0.5f }, { 0.25f, 0.25f }, mSquareColor2);
		//Labyrinth::Renderer2D::DrawQuad({ 0.75f, 0.75f }, { 0.5f, 0.5f }, mSquareColor);
		//Labyrinth::Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 0.1f, 0.1f }, mSquareColor2);
		//Labyrinth::Renderer2D::DrawQuad({ 0.0f, 0.0f}, { 1.8f, 1.8f }, mCheckerboardTexture);
		//Labyrinth::Renderer2D::DrawRotatedQuad({ 2.0f, 2.0f }, { 1.0f, 1.0f }, rotation, mCheckerboardTexture, 2.0f);
		Labyrinth::Renderer2D::EndState();


		Labyrinth::Renderer2D::BeginState(mCameraController.getCamera());
		for (float y = -5.0f; y < 5.0f; y += 0.5f)
		{
			for (float x = -5.0f; x < 5.0f; x += 0.5f)
			{
				glm::vec4 colour = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };
				Labyrinth::Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, colour);
			}
		}
		Labyrinth::Renderer2D::EndState();

	}
}

void Sandbox2D::onImGuiRender()
{
	LAB_PROFILE_FUNCTION();

	Labyrinth::Renderer2D::ResetStats();

	ImGui::Begin("Settings");

	static bool demo = true; //Enable demo
	ImGui::ShowDemoWindow(&demo);

	auto stats = Labyrinth::Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.drawCalls);
	ImGui::Text("Quads: %d", stats.quadCount);
	ImGui::Text("Vertices: %d", stats.getTotalVertexCount());
	ImGui::Text("Indices: %d", stats.getTotalIndexCount());


	ImGui::Dummy(ImVec2(0.0f, 20.0f));

	ImGui::ColorEdit4("Square Color", glm::value_ptr(mSquareColor));
	ImGui::ColorEdit4("Square Color 2", glm::value_ptr(mSquareColor2));

	uint32_t textureID = mCheckerboardTexture->getRendererID();
	ImGui::Image((void*)textureID, ImVec2{ 1280, 720 });

	ImGui::End();
}

void Sandbox2D::onEvent(Labyrinth::Event& e)
{
	mCameraController.onEvent(e);
}
