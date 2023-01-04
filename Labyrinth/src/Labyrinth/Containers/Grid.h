#pragma once

#include <vector>

#include <Labyrinth/Core/System/Base.h>

namespace Laby {

	template<typename T>
	class Grid
	{
	public:
		struct GridPos 
		{ 
			usize x = Limits::usizeMax, y = Limits::usizeMax;

			GridPos() = default;
			GridPos(usize _x, usize _y) : x(_x), y(_y) {} 

			bool operator==(const GridPos& other) const { return (x == other.x && y == other.y); }
			bool operator<(const GridPos& other) const
			{
				if (y == other.y)
					return x < other.x;
				return y < other.y;
			}
		};

		using GridBool = u8;
		static constexpr int GridTrue = 1;
		static constexpr int GridFalse = 0;

	public:
		Grid(usize width, usize height)
			: mWidth(width), mHeight(height), mData(width * height)
		{}

		T& operator()(usize x, usize y) { return mData[x + (mWidth * y)]; }
		const T& operator()(usize x, usize y) const { return mData[x + (mWidth * y)]; }

		T& operator()(const GridPos& pos) { return mData[pos.x + (mWidth * pos.y)]; }
		const T& operator()(const GridPos& pos) const { return mData[pos.x + (mWidth * pos.y)]; }

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

		T& At(const GridPos& pos) { return mData[pos.x + (mWidth * pos.y)]; }
		const T& At(const GridPos& pos) const { return mData[pos.x + (mWidth * pos.y)]; }

	protected:
		usize mWidth = 0, mHeight = 0;

	private:
		std::vector<T> mData;
	};

}