#pragma once

namespace Labyrinth {

	class ModalWindow
	{
	public:
		ModalWindow(void* inData = nullptr);

		virtual void display() = 0;
		bool complete() { return mComplete; }

	protected:
		virtual void Close();

	protected:
		void* mPayload; // For use returning data to the screen that called the modal
		bool mComplete = false;
	};

}