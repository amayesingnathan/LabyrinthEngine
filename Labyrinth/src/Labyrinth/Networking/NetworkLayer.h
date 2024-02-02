#pragma once

#include "Common/Application.h"

namespace Laby {

	class NetworkLayer : public slc::ApplicationLayer
	{
	public:
		virtual void Send(const Message& msg) = 0;
	};
}