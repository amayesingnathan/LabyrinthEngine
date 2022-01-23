#pragma once

#include <xhash>
#include <string>

namespace Labyrinth {

	class UUID
	{
	public:
		UUID();
		UUID(uint64_t id);
		UUID(const UUID&) = default;

		operator uint64_t() const { return mID; }

		std::string to_string() const { return std::to_string(mID); }

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