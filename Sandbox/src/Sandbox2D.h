#pragma once

#include "Labyrinth.h"

class Sandbox2D : public Labyrinth::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;

	virtual void onAttach() override;
	virtual void onDetach() override;

	void onUpdate(Labyrinth::Timestep ts) override;
	virtual void onImGuiRender() override;
	void onEvent(Labyrinth::Event& e) override;

private:
	Labyrinth::OrthographicCameraController mCameraController;

	Labyrinth::Ref<Labyrinth::Texture2D> mCheckerboardTexture;

	Labyrinth::Ref<Labyrinth::Texture2DSheet> mSpriteSheet;
	Labyrinth::Ref<Labyrinth::SubTexture2D> mTNT;

	glm::vec4 mSquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
	glm::vec4 mSquareColor2 = { 0.8f, 0.5f, 0.1f, 1.0f };
};