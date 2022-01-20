#pragma once

#include "NetCommon.h"

namespace Labyrinth {

	namespace Net
	{
		template<typename T>
		class TSQueue
		{
		public:
			TSQueue() = default;
			TSQueue(const TSQueue<T>&) = delete;
			virtual ~TSQueue() { clear(); }

		public:
			const T& front()
			{
				std::scoped_lock lock(mMutex);
				return mQueue.front();
			}

			const T& back()
			{
				std::scoped_lock lock(mMutex);
				return mQueue.back();
			}

			void push_back(const T& item)
			{
				std::scoped_lock lock(mMutex);
				return mQueue.emplace_back(std::move(item));
			}

			void push_front(const T& item)
			{
				std::scoped_lock lock(mMutex);
				return mQueue.emplace_front(std::move(item));
			}

			bool empty()
			{
				std::scoped_lock lock(mMutex);
				return mQueue.empty();
			}

			size_t count()
			{
				std::scoped_lock lock(mMutex);
				return mQueue.size();
			}

			void clear()
			{
				std::scoped_lock lock(mMutex);
				return mQueue.clear();
			}

			T pop_front()
			{
				std::scoped_lock lock(mMutex);
				auto t = std::move(mQueue.front());
				mQueue.pop_front();
				return t;
			}

			T pop_back()
			{
				std::scoped_lock lock(mMutex);
				auto t = std::move(mQueue.back());
				mQueue.pop_back();
				return t;
			}

		protected:
			std::mutex mMutex;
			std::deque<t> mQueue;
		};

	}
}