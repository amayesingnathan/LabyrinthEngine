#pragma once

#include <Labyrinth/Core/System/Base.h>

namespace Laby {

	class UUID
	{
	public:
		UUID();
		UUID(u64 id);

		operator u64() const { return mID; }

		std::string to_string() const { return std::to_string(mID); }

	private:
		u64 mID;
	};
}

namespace std {
	template<typename T> struct hash;

	template<>
	struct hash<Laby::UUID>
	{
		std::size_t operator()(const Laby::UUID& uuid) const
		{
			return (Laby::u64)uuid;
		}
	};

}