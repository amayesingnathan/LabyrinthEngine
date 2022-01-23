#pragma once

#include <xhash>

namespace Labyrinth {

	class UUID
	{
	public:
		UUID();
		UUID(uint64_t id);
		UUID(const UUID&) = default;

		operator uint64_t() const { return mID; }

	private:
		uint64_t mID;

	};

}

namespace std {

	template<>
	struct hash<Labyrinth::UUID>
	{
		std::size_t operator()(const Labyrinth::UUID& uuid) const
		{
			return hash<uint64_t>()((uint64_t)uuid);
		}
	};

}