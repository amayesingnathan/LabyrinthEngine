#pragma once

#include <queue>
#include <mutex>

namespace Laby {

	template<typename T>
	class TSQueue
	{
	public:
		TSQueue() = default;
		TSQueue(const TSQueue<T>&) = delete;
		~TSQueue() { clear(); }

	public:
		const T& front() const
		{
			std::scoped_lock lock(mQueueMutex);
			return mQueue.front();
		}

		const T& back() const
		{
			std::scoped_lock lock(mQueueMutex);
			return mQueue.back();
		}

		T pop()
		{
			std::scoped_lock lock(mQueueMutex);
			auto t = std::move(mQueue.front());
			mQueue.pop();
			return t;
		}

		void push(const T& item)
		{
			std::scoped_lock lock(mQueueMutex);
			mQueue.push(item);
		}

		template<typename... Args>
		void emplace(Args&&... args)
		{
			std::scoped_lock lock(mQueueMutex);
			mQueue.emplace(std::forward<Args>(args)...);
		}

		bool empty()
		{
			std::scoped_lock lock(mQueueMutex);
			return mQueue.empty();
		}

		usize count()
		{
			std::scoped_lock lock(mQueueMutex);
			return mQueue.size();
		}

		void clear()
		{
			std::scoped_lock lock(mQueueMutex);
			mQueue = {};
		}

	protected:
		std::queue<T> mQueue;
		mutable std::mutex mQueueMutex;
	};

}