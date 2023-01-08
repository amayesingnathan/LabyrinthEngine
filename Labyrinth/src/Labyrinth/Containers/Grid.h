#pragma once

#include <vector>

#include <Labyrinth/Core/System/Base.h>

namespace Laby {

	template<typename T>
	concept IsIntegral = std::is_integral_v<T>;

	template<IsIntegral T = usize>
	struct GridPos
	{
		usize x = Limits<T>::Max, y = Limits<T>::Max;

		GridPos() = default;
		constexpr GridPos(T _x, T _y) : x(_x), y(_y) {}

		bool operator==(const GridPos& other) const { return (x == other.x && y == other.y); }
		bool operator<(const GridPos& other) const
		{
			if (y == other.y)
				return x < other.x;
			return y < other.y;
		}
	};

	using GridPosition = GridPos<>;

	template<typename T>
	class Grid
	{
	public:
		using Position = GridPos<>;

		using GridBool = u8;
		static constexpr int GridTrue = 1;
		static constexpr int GridFalse = 0;

	public:
		Grid() = default;
		Grid(usize width, usize height)
			: mWidth(width), mHeight(height), mData(width * height)
		{}

		T& operator()(usize x, usize y) { return mData[x + (mWidth * y)]; }
		const T& operator()(usize x, usize y) const { return mData[x + (mWidth * y)]; }

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
		T& At(usize x, usize y) { return mData[x + (mWidth * y)]; }
		const T& At(usize x, usize y) const { return mData[x + (mWidth * y)]; }

		T& At(const Position& pos) { return mData[pos.x + (mWidth * pos.y)]; }
		const T& At(const Position& pos) const { return mData[pos.x + (mWidth * pos.y)]; }

	protected:
		usize mWidth = 0, mHeight = 0;

	private:
		std::vector<T> mData;
	};

}