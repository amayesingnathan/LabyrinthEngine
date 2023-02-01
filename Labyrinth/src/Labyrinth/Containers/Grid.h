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

		using Bool = u8;
		static constexpr Bool True = 1;
		static constexpr Bool False = 0;

	public:
		Grid() = default;
		Grid(usize width, usize height)
			: mWidth(width), mHeight(height), mData(width * height)
		{}

		T& operator()(TPos x, TPos y) { return mData[x + (mWidth * y)]; }
		const T& operator()(TPos x, TPos y) const { return mData[x + (mWidth * y)]; }

		T& operator()(const Position& pos) { return mData[pos.x + (mWidth * pos.y)]; }
		const T& operator()(const Position& pos) const { return mData[pos.x + (mWidth * pos.y)]; }

		void set(usize index, const T& data) { mData[index] = data; }

		usize getWidth() const { return mWidth; }
		usize getHeight() const { return mHeight; }

		void resize(usize width, usize height) { mData.clear(); mData.resize(width * height); }

		auto begin() { return mData.begin(); }
		auto begin() const { return mData.cbegin(); }
		auto end() { return mData.end(); }
		auto end() const { return mData.cend(); }

	protected:
		T& At(TPos x, TPos y) { return mData[x + (mWidth * y)]; }
		const T& At(TPos x, TPos y) const { return mData[x + (mWidth * y)]; }

		T& At(const Position& pos) { return mData[pos.x + (mWidth * pos.y)]; }
		const T& At(const Position& pos) const { return mData[pos.x + (mWidth * pos.y)]; }

	protected:
		usize mWidth = 0, mHeight = 0;

	private:
		std::vector<T> mData;
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