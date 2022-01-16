#pragma once

#include "Labyrinth/Core/System/Assert.h"

namespace Labyrinth {

	/*
		Casting Tools
	*/

	template<typename T>
	constexpr T* Cast(void* target)
	{
		return static_cast<T*>(target);
	}
	template<typename To, typename From>
	constexpr To Cast(From target)
	{
		return static_cast<To>(target);
	}
	template<typename To, typename From>
	constexpr To* Cast(From* target)
	{
		return static_cast<To*>(target);
	}

	/*
		Works the same as dynamic_cast but should never need to use * or & in the template type.
		It will be determined based on whether the argument is a * or &.

		Base b;
		Derived d;
		Base* bAdr = &b;
		Derived* dAdr = &d;

		Base baseCopy = CastToRelative<Base>(d);
		Base& baseRef = CastToRelative<Base>(d);
		Base* basePtr = CastToRelative<Base>(dAdr);


		Can only downcast if dealing with a pointer to an instance of the derived type.
			i.e. CastToRelative<Derived>(b); will trigger an assert

		bAdr = &d;
		Derived derivedCopy = CastToRelative<Derived>(*bAdr);
		Derived& derivedRef = CastToRelative<Derived>(*bAdr);
		Derived* derivedPtr = CastToRelative<Derived>(bAdr);

	*/
	template<typename To, typename From>
	constexpr To* CastToRelative(From* targetPointer)
	{
		bool isRelated = std::is_base_of<From, To>::value || std::is_base_of<To, From>::value;
		LAB_CORE_ASSERT(isRelated, "Cannot cast to type that is not a sub class!");
		return dynamic_cast<To*>(targetPointer);
	}

	template<typename To, typename From>
	constexpr To& CastToRelative(From& target)
	{
		bool isRelated = std::is_base_of<From, To>::value || std::is_base_of<To, From>::value;
		LAB_CORE_ASSERT(isRelated, "Cannot cast to type that is not a sub class!");

		From* targetPtr = &target;
		To* toPtr = dynamic_cast<To*>(targetPtr);
		LAB_CORE_ASSERT(toPtr, "Bad cast! Return value was null!");

		return *toPtr;
	}

	template<typename To, typename From>
	constexpr Ref<To> CastRef(Ref<From> target)
	{
		return std::static_pointer_cast<To>(target);
	}

	template<typename To, typename From>
	constexpr Ref<To> CastRefToRelative(Ref<From> target)
	{
		bool isRelated = std::is_base_of<From, To>::value || std::is_base_of<To, From>::value;
		LAB_CORE_ASSERT(isRelated, "Cannot cast to type that is not a sub class!");
		return std::dynamic_pointer_cast<To>(target);
	}

}