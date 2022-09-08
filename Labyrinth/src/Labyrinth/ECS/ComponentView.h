#pragma once

#include "ComponentPool.h"

#include <Labyrinth/Core/System/Ref.h>

#include <array>

namespace Labyrinth::ECS {

	template<typename... T>
	class ComponentView
	{
	public:
		using ComponentList = TypeList<T...>;
		using ComponentTuple  = typename ComponentList::Tuple;

	public:
		ComponentView(std::array<Ref<IComponentPool>, ComponentList::Size>& pools) 
		{
			for (usize i = 0; i < ComponentList::Size; i++)
				mPools[i] = std::move(pools[i]);
		}

		void each(std::function<void(T&...)> function)
		{
			Ref<IComponentPool> smallestPool = GetSmallest();
			for (EntityID entity : *smallestPool)
			{
				bool validEntity = true;
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

				if (!validEntity)
					continue;

				function(std::get<T>(GenTuple(entity))...);
			}
		}

	private:
		Ref<IComponentPool> GetSmallest() const
		{
			auto it = std::min_element(mPools.begin(), mPools.end(), [](const Ref<IComponentPool>& pool1, const Ref<IComponentPool>& pool2) 
			{
				return pool1->size() < pool2->size();
			});
			if (it == mPools.end())
				return nullptr;

			return *it;
		}

		template<usize index>
		auto& ToTupleElement(EntityID entity) const
		{
			using ElementType = typename std::tuple_element<index, ComponentTuple>::type;
			Ref<ComponentPool<ElementType>> pool = mPools[index];
			return pool->get(entity);
		}

		template <usize... Is>
		auto GenTupleImpl(EntityID entity, std::index_sequence<Is...>)
		{
			return std::make_tuple(ToTupleElement<Is>(entity)...);
		}

		auto GenTuple(EntityID entity)
		{
			return GenTupleImpl(entity, std::make_index_sequence<ComponentList::Size>{});
		}

	private:
		std::array<Ref<IComponentPool>, ComponentList::Size> mPools;
	};

}