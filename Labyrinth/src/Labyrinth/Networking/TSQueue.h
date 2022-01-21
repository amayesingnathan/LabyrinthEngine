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
				std::scoped_lock lock(mQueueMutex);
				return mQueue.front();
			}

			const T& back()
			{
				std::scoped_lock lock(mQueueMutex);
				return mQueue.back();
			}

			T pop_front()
			{
				std::scoped_lock lock(mQueueMutex);
				auto t = std::move(mQueue.front());
				mQueue.pop_front();
				return t;
			}

			T pop_back()
			{
				std::scoped_lock lock(mQueueMutex);
				auto t = std::move(mQueue.back());
				mQueue.pop_back();
				return t;
			}

			void push_back(const T& item)
			{
				std::scoped_lock lock(mQueueMutex);
				mQueue.emplace_back(std::move(item));

				std::unique_lock loc(mBlockingMutex);
				mBlocking.notify_one();
			}

			void push_front(const T& item)
			{
				std::scoped_lock lock(mQueueMutex);
				mQueue.emplace_front(std::move(item));

				std::unique_lock loc(mBlockingMutex);
				mBlocking.notify_one();
			}

			bool empty()
			{
				std::scoped_lock lock(mQueueMutex);
				return mQueue.empty();
			}

			size_t count()
			{
				std::scoped_lock lock(mQueueMutex);
				return mQueue.size();
			}

			void clear()
			{
				std::scoped_lock lock(mQueueMutex);
				return mQueue.clear();
			}

			void wait()
			{
				while (empty())
				{
					std::unique_lock<std::mutex> ul(mBlockingMutex);
					mBlocking.wait(ul);
				}
			}

		protected:
			std::mutex mQueueMutex;
			std::deque<T> mQueue;
			std::condition_variable mBlocking;
			std::mutex mBlockingMutex;
		};

	}
}