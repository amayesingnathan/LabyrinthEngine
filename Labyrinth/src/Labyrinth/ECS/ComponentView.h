#pragma once

#include "ComponentPool.h"
#include "ComponentList.h"

#include <Labyrinth/Core/System/Ref.h>

#include <array>

namespace Labyrinth::ECS {

	template <typename F, usize... Is>
	auto gen_tuple_impl(F func, std::index_sequence<Is...>) 
	{
		return std::make_tuple(func(Is)...);
	}

	template <usize N, typename F>
	auto gen_tuple(F func) 
	{
		return gen_tuple_impl(func, std::make_index_sequence<N>{});
	}

	template<typename... T>
	class ComponentView
	{
	public:
		using ComponentList = TypeList<T...>;

	public:
		ComponentView(std::array<Ref<IComponentPool>, ComponentList::Size>& pools) 
		{
			for (usize i = 0; i < ComponentList::Size; i++)
				mPools[i] = std::move(pools[i]);
		}

		void each(std::function<void(ComponentList::Tuple)> function)
		{
			Ref<IComponentPool> smallestPool = GetSmallest();
			for (EntityID entity : smallestPool)
			{
				bool validEntity = true;
				ComponentList::Tuple parameters;

				for (Ref<IComponentPool> pool : mPools)
				{
					if (pool == smallestPool)
						continue;

					if (!pool->exists(entity))
					{
						validEntity = false;
						break;
					}
				}

				//////////////TODO//////////////
				ComponentList::Tuple params = gen_tuple<ComponentList::Size>([](usize index) {});
				////////////////////////////////

				if (valid)
					function(parameters)
			}
		}

	private:
		Ref<IComponentPool> GetSmallest() const
		{
			auto it = std::min_element(mPools.begin(), mPools.end(), [](const Ref<IComponentPool>& pool1, const Ref<IComponentPool>& pool2) 
			{
				pool1->size() < pool2->size();
			});
			if (it == mPools.end())
				return nullptr;

			return *it;
		}

	private:
		std::array<Ref<IComponentPool>, ComponentList::Size> mPools;
	};

}