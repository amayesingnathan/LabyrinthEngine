#pragma once

#include <Labyrinth/Core/System/Types.h>

#include <string>

namespace Labyrinth {

	class LAB_API UUID
	{
	public:
		UUID();
		UUID(u64 id);
		UUID(const UUID&) = default;

		operator u64() const { return mID; }

		std::string to_string() const { return std::to_string(mID); }

	private:
		u64 mID;

	};

}

namespace std {
	template<typename T> struct hash;

	template<>
	struct hash<Labyrinth::UUID>
	{
		std::size_t operator()(const Labyrinth::UUID& uuid) const
		{
			return (u64)uuid;
		}
	};

}