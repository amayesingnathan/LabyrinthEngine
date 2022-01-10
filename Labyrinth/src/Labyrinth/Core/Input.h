#pragma once

namespace Labyrinth {

	class Input
	{
	protected:
		Input() = default;
	public:
		virtual ~Input() = default;

		Input(const Input&) = delete;
		Input& operator=(const Input&) = delete;

		static bool IsKeyPressed(int keycode) { return sInstance->isKeyPressedImpl(keycode); }

		static bool IsMouseButtonPressed(int button) { return sInstance->isMouseButtonPressedImpl(button); }
		static std::pair<float, float> GetMousePosition() { return sInstance->getMousePositionImpl(); }
		static float GetMouseX() { return sInstance->getMouseXImpl(); }
		static float GetMouseY() { return sInstance->getMouseYImpl(); }

	protected:
		virtual bool isKeyPressedImpl(int keycode) = 0;

		virtual bool isMouseButtonPressedImpl(int keycode) = 0;
		virtual std::pair<float, float> getMousePositionImpl() = 0;
		virtual float getMouseXImpl() = 0;
		virtual float getMouseYImpl() = 0;

	private:
		static Single<Input> sInstance;
	};

}