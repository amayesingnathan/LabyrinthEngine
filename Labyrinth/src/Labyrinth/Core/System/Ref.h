#pragma once

#include <unordered_set>

#include "Base.h"

namespace Laby {

	class RefCounted;

	template<typename T>
	concept RefCountable = DerivedFrom<RefCounted, T>;

	class RefCounted
	{
	public:
		u32 getRefCount() const { return mRefCount; }

	private:
		void incRefCount() const { ++mRefCount; }
		void decRefCount() const { --mRefCount; }

	private:
		mutable u32 mRefCount = 0;

		template<RefCountable T>
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

		template<RefCountable T>
		friend class Ref;
	};

	template<RefCountable T>
	class Ref
	{
	public:
		Ref() : mData(nullptr) {}
		Ref(std::nullptr_t) : mData(nullptr) {}
		Ref(T* data) : mData(data) { IncRef(); }

		template<typename Other>
		Ref(const Ref<Other>& other)
		{
			mData = (T*)other.mData;
			IncRef();
		}

		template<typename Other>
		Ref(Ref<Other>&& other)
		{
			mData = (T*)other.mData;
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

			mData = (T*)other.mData;
			return *this;
		}

		template<typename Other>
		Ref& operator=(const Ref<Other>& other)
		{
			other.IncRef();
			DecRef();

			mData = (T*)other.mData;
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

		operator bool() const { return mData != nullptr; }

		T* operator->() { return mData; }
		const T* operator->() const { return mData; }

		T& operator*() { return *mData; }
		const T& operator*() const { return *mData; }

		T* data() { return mData; }
		const T* data() const { return mData; }

		bool operator==(const Ref<T>& other) const { return mData == other.mData; }
		bool operator==(std::nullptr_t) const { return mData == nullptr; }

		void reset()
		{
			DecRef();
			mData = nullptr;
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
			if (!mData) 
				return;

			mData->incRefCount();
			RefTracker::AddToTrackedRefs((void*)mData);
		}

		void DecRef() const
		{
			if (!mData) 
				return;

			mData->decRefCount();
			if (mData->getRefCount() == 0)
			{
				delete mData;
				RefTracker::RemoveFromTrackedRefs(mData);
				mData = nullptr;
			}
		}

	private:
		template<RefCountable Other>
		friend class Ref;

		mutable T* mData;
	};


	template<RefCountable T>
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

namespace std {
	template<typename T> struct hash;

	template<typename T>
	struct hash<Laby::Ref<T>>
	{
		std::size_t operator()(const Laby::Ref<T>& ref) const
		{
			return std::hash<const void*>()((const void*)ref.data());
		}
	};

}