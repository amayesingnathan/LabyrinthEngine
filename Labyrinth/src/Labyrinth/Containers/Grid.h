#pragma once

#include <vector>

#include <Labyrinth/Core/System/Base.h>
#include <Labyrinth/IO/YAML.h>

namespace Laby {

	template<std::integral T>
	struct Coordinate
	{
		static constexpr T NullCoord = Limits<T>::Max;
		T x = NullCoord, y = NullCoord;

		Coordinate() = default;
		constexpr Coordinate(T _x, T _y) : x(_x), y(_y) {}

		bool operator==(const Coordinate& other) const { return (x == other.x && y == other.y); }
		bool operator<(const Coordinate& other) const
		{
			if (y == other.y)
				return x < other.x;
			return y < other.y;
		}

		bool valid() const { return x != NullCoord && y != NullCoord;}
	};

	using GridPosition = Coordinate<u32>;

	template<typename T, std::integral TPos = u32>
	class Grid
	{
	public:
		using Position = Coordinate<TPos>;

	private:
		using Bool = u8;
		static constexpr Bool True = 1;
		static constexpr Bool False = 0;

		static constexpr bool IsBool = std::is_same_v<T, bool>;

		using Type = std::conditional_t<IsBool, Bool, T>;
		using TReturn = std::conditional_t<IsBool, Bool&, T&>;
		using TConstReturn = std::conditional_t<IsBool, bool, const T&>;

	public:
		Grid() = default;
		Grid(usize width, usize height)
			: mWidth(width), mHeight(height), mData(width * height)
		{}

		TReturn operator()(TPos x, TPos y) { return mData[x + (mWidth * y)]; }
		TConstReturn operator()(TPos x, TPos y) const 
		{
			if constexpr (IsBool)
				return (bool)mData[x + (mWidth * y)];

			return mData[x + (mWidth * y)]; 
		}

		TReturn operator()(const Position& pos){ return mData[pos.x + (mWidth * pos.y)]; }
		TConstReturn operator()(const Position& pos) const
		{
			if constexpr (IsBool)
				return (bool)mData[pos.x + (mWidth * pos.y)];

			return mData[pos.x + (mWidth * pos.y)];
		}

		void set(usize index, const T& data) 
		{ 
			if constexpr (IsBool)
				mData[index] = data ? True : False;
			else
				mData[index] = data; 
		}

		usize getWidth() const { return mWidth; }
		usize getHeight() const { return mHeight; }

		void resize(usize width, usize height) { mData.clear(); mData.resize(width * height); }
		void reset() { mData.clear(); mData.resize(mWidth * mHeight); }

		auto begin() { return mData.begin(); }
		auto begin() const { return mData.cbegin(); }
		auto end() { return mData.end(); }
		auto end() const { return mData.cend(); }

	protected:
		usize mWidth = 0, mHeight = 0;

		TReturn At(TPos x, TPos y) { return mData[x + (mWidth * y)]; }
		TConstReturn At(TPos x, TPos y) const
		{
			if constexpr (IsBool)
				return (bool)mData[x + (mWidth * y)];

			return mData[x + (mWidth * y)];
		}

		TReturn At(const Position& pos) { return mData[pos.x + (mWidth * pos.y)]; }
		TConstReturn At(const Position& pos) const
		{
			if constexpr (IsBool)
				return (bool)mData[pos.x + (mWidth * pos.y)];

			return mData[pos.x + (mWidth * pos.y)];
		}

	private:
		std::vector<Type> mData;
	};
	
	template<typename T>
	inline YAML::Emitter& operator<<(YAML::Emitter& mOut, const Coordinate<T>& data)
	{
		mOut << YAML::Flow;
		mOut << YAML::BeginSeq << data.x << data.y << YAML::EndSeq;
		return mOut;
	}
}

namespace YAML {

	template<typename T>
	struct convert<Laby::Coordinate<T>>
	{
		inline static bool decode(const Node& node, Laby::Coordinate<T>& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<T>();
			rhs.y = node[1].as<T>();
			return true;
		}
	};
}

namespace std {
	template<typename T> struct hash;

	template<typename T>
	struct hash<Laby::Coordinate<T>>
	{
		std::size_t operator()(const Laby::Coordinate<T>& pos) const
		{
			Laby::u64 combo = (Laby::u64)pos.x | (Laby::u64)pos.y << 32;
			return std::hash<Laby::u64>()(combo);
		}
	};
}