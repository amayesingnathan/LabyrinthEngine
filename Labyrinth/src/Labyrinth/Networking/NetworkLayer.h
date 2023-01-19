#pragma once

#include <Labyrinth/Core/Layer.h>

namespace Laby {

	class NetworkLayer : public Layer
	{
	public:
		virtual void send(const Message& msg) = 0;
	};
}