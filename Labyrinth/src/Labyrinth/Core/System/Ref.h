#pragma once

#include <unordered_set>

namespace Labyrinth {

	class RefCounted
	{
	public:
		u32 getRefCount() const { return mRefCount; }

	private:
		void incRefCount() const
		{
			++mRefCount;
		}
		void decRefCount() const
		{
			--mRefCount;
		}

	private:
		mutable u32 mRefCount = 0;

		template<typename T>
		friend class Ref;
	};

	class RefTracker
	{
	public:
		static bool IsTracked(void* data);

	private:
		static void AddToTrackedRefs(void* data);
		static void RemoveFromTrackedRefs(void* data);

	private:
		using RefSet = std::unordered_set<void*>;
		inline static RefSet sRefSet;

		template<typename T>
		friend class Ref;
	};

	template<typename T>
	class Ref
	{
	public:
		Ref() : mData(nullptr) {}
		Ref(std::nullptr_t) : mData(nullptr) {}
		Ref(T* data) : mData(data) { IncRef(); }

		template<typename Other>
		Ref(const Ref<Other>& other)
		{
			mData = CastToRelative<T>(other.mData);
			IncRef();
		}

		template<typename Other>
		Ref(Ref<Other>&& other)
		{
			mData = CastToRelative<T>(other.mData);
			other.mData = nullptr;
		}

		Ref(const Ref<T>& other)
			: mData(other.mData)
		{
			IncRef();
		}

		~Ref() { DecRef(); }

		Ref& operator=(std::nullptr_t)
		{
			DecRef();
			mData = nullptr;
			return *this;
		}

		Ref& operator=(const Ref<T>& other)
		{
			other.IncRef();
			DecRef();

			mData = other.mData;
			return *this;
		}

		template<typename Other>
		Ref& operator=(const Ref<Other>& other)
		{
			other.IncRef();
			DecRef();

			mData = other.mData;
			return *this;
		}

		template<typename Other>
		Ref& operator=(Ref<Other>&& other)
		{
			DecRef();

			mData = other.mData;
			other.mData = nullptr;
			return *this;
		}

		bool operator==(const Ref<T>& other) const
		{
			return mData == other.mData;
		}

		bool operator!=(const Ref<T>& other) const
		{
			return !(*this == other);
		}

		operator bool() const { return mData != nullptr; }

		T* operator->() { return mData; }
		const T* operator->() const { return mData; }

		T& operator*() { return *mData; }
		const T& operator*() const { return *mData; }

		T* data() { return mData; }
		const T* data() const { return mData; }

		void reset(T* instance = nullptr)
		{
			DecRef();
			mData = instance;
		}

		template<typename Other>
		Ref<Other> to() const
		{
			return Ref<Other>(*this);
		}

		template<typename... Args>
		static Ref<T> Create(Args&&... args)
		{
			return Ref<T>(new T(std::forward<Args>(args)...));
		}

	private:
		void IncRef() const
		{
			if (!mData) return;

			mData->incRefCount();	
			RefTracker::AddToTrackedRefs((void*)mData);
		}

		void DecRef() const
		{
			if (!mData) return;

			mData->decRefCount();
			if (mData->getRefCount() == 0)
			{
				delete mData;
				RefTracker::RemoveFromTrackedRefs(mData);
				mData = nullptr;
			}
		}

	private:
		template<typename Other>
		friend class Ref;

		mutable T* mData;
	};

	template<typename T>
	class WeakRef
	{
	public:
		WeakRef() = default;

		WeakRef(Ref<T> ref)
		{
			mData = ref.data();
		}

		WeakRef(T* instance)
		{
			mData = instance;
		}

		T* operator->() { return mData; }
		const T* operator->() const { return mData; }

		T& operator*() { return *mData; }
		const T& operator*() const { return *mData; }

		bool valid() const { return mData ? RefTracker::IsTracked(mData) : false; }
		operator bool() const { return valid(); }

		Ref<T> lock() const
		{
			return Ref<T>(mData);
		}

	private:
		T* mData = nullptr;
	};
}