#pragma once

#include <Labyrinth/Core/System/Base.h>
#include <Labyrinth/IO/YAML.h>

namespace Laby {

	class UUID
	{
	public:
		UUID();
		constexpr UUID(u64 id)
			: mID(id) {}

		u64 Get() const { return mID; }

		operator bool() const { return mID != 0; }
		auto operator<=>(const UUID&) const = default;
		auto operator<=>(std::unsigned_integral auto val) const { return mID <=> val; }

		std::string ToString() const { return std::to_string(mID); }

	private:
		u64 mID;
	};

	inline YAML::Emitter& operator<<(YAML::Emitter& mOut, const UUID& data)
	{
		mOut << data.Get();
		return mOut;
	}
}

namespace YAML {

	template<>
	struct convert<Laby::UUID>
	{
		inline static bool decode(const Node& node, Laby::UUID& rhs)
		{
			rhs = node.as<Laby::u64>();
			return true;
		}
	};
}

namespace std {
	template<typename T> struct hash;

	template<>
	struct hash<Laby::UUID>
	{
		std::size_t operator()(const Laby::UUID& uuid) const
		{
			return uuid.Get();;
		}
	};

}