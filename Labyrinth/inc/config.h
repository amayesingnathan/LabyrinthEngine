#pragma once

struct configuration {
	static constexpr int FPS = 60;
	static constexpr float frameDelay = static_cast<float>(1) / static_cast<float>(FPS);
	static constexpr int mFrameDelay = 1000 / FPS;

	static constexpr int SCREEN_WIDTH = 800;
	static constexpr int SCREEN_HEIGHT = 640;
};