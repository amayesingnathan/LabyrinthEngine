#pragma once

#include "Labyrinth/Core/Layer.h"

namespace Labyrinth {

	namespace Net {

		class LAB_API NetworkLayer : public Layer 
		{
		public:
			virtual void onMessage(Ref<Connection> from, Message& msg) = 0;
		};

	}

}