#pragma once

#include <vector>

#include <Labyrinth/Core/System/Base.h>

namespace Laby {

	template<typename T>
	class Grid
	{
	public:
		Grid(usize width, usize height)
			: mWidth(width), mHeight(height), mData(width * height)
		{}

		T& operator()(usize x, usize y) { return mData[x + (mWidth * y)]; }
		const T& operator()(usize x, usize y) const { return mData[x + (mWidth * y)]; }

		void set(usize index, const T& data) { mData[index] = data; }

		usize getWidth() const { return mWidth; }
		usize getHeight() const { return mHeight; }

		void resize(usize width, usize height) { mData.clear(); mData.resize(width * height); }

		auto begin() { return mData.begin(); }
		auto begin() const { return mData.cbegin(); }
		auto end() { return mData.end(); }
		auto end() const { return mData.cend(); }

	private:
		usize mWidth = 0, mHeight = 0;
		std::vector<T> mData;
	};

}