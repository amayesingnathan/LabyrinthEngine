#pragma once

#include <Labyrinth/Core/System/Base.h>

namespace Laby {

	struct IPayload
	{
		virtual void* data() = 0;
		virtual usize size() const = 0;
	};

	template<typename T>
	struct Payload : public IPayload
	{
		using Type = std::decay_t<T>;

		Payload(const Type& data) : mData(data) {}

		void* data() override { return &mData; }
		usize size() const override { return sizeof(T); }

	private:
		Type mData;
	};
}