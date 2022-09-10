#pragma once

#include "ComponentPool.h"

#include <Labyrinth/Core/System/Ref.h>
#include <Labyrinth/Core/System/Reflection.h>

#include <array>

namespace Labyrinth::ECS {

	template<typename... T>
	class ComponentView
	{
	public:
		using ComponentList = TypeList<T...>;
		using ComponentTuple  = typename ComponentList::TupleType;

	public:
		ComponentView(std::array<Ref<IComponentPool>, ComponentList::Size>& pools) 
		{
			for (usize i = 0; i < ComponentList::Size; i++)
				mPools[i] = std::move(pools[i]);

			BuildEntityList();
		}

		void each(std::function<void(EntityID, T&...)> function)
		{
			for (EntityID entity : mValidEntities)
			{
				// Create tuple of components and unpack into function
				function(entity, std::get<T&>(GenTuple(entity))...);
			}
		}

		template<typename U>
		U& get(EntityID entity)
		{
			LAB_STATIC_ASSERT(ComponentList::Contains<U>, "Template is not a component in this view!");
			LAB_CORE_ASSERT(ValidEntity(entity), "This entity does not have all the required components!");

			Ref<ComponentPool<U>> pool = mPools[ComponentList::Index<U>];
			return pool->get(entity);
		}

		usize size() const { return mValidEntities.size(); }

		auto begin() { return mValidEntities.begin(); }
		auto begin() const { return mValidEntities.cbegin(); }
		auto end() { return mValidEntities.end(); }
		auto end() const { return mValidEntities.cend(); }

	private:
		Ref<IComponentPool> GetSmallest()
		{
			auto it = std::min_element(mPools.begin(), mPools.end(), [](const Ref<IComponentPool>& pool1, const Ref<IComponentPool>& pool2) 
			{
				return pool1->size() < pool2->size();
			});
			if (it == mPools.end())
				return nullptr;

			return *it;
		}

		void BuildEntityList()
		{
			mValidEntities.clear();
			Ref<IComponentPool> smallestPool = GetSmallest();

			for (EntityID entity : *smallestPool)
			{
				if (ValidEntity(entity, smallestPool))
					mValidEntities.push_back(entity);
			}
		}

		template<usize index>
		auto& ToTupleElement(EntityID entity)
		{
			using ElementType = typename ComponentList::Type<index>;
			Ref<ComponentPool<ElementType>> pool = mPools[index];
			return pool->get(entity);
		}

		bool ValidEntity(EntityID entity, const Ref<IComponentPool>& smallestPool)
		{
			for (Ref<IComponentPool> pool : mPools)
			{
				if (pool == smallestPool)
					continue;

				if (!pool->exists(entity))
					return false;
			}

			return true;
		}
		bool ValidEntity(EntityID entity)
		{
			return ValidEntity(entity, GetSmallest());
		}

		template <usize... Is>
		auto GenTupleImpl(EntityID entity, std::index_sequence<Is...>) { return std::tie(ToTupleElement<Is>(entity)...); }

		auto GenTuple(EntityID entity) { return GenTupleImpl(entity, std::make_index_sequence<ComponentList::Size>{}); }

	private:
		std::array<Ref<IComponentPool>, ComponentList::Size> mPools;
		std::vector<EntityID> mValidEntities;
	};

}