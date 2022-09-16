#pragma once

#include "ComponentPool.h"

namespace Labyrinth::ECS {

	// Using a view when changes were made to the registry after instantiation of the view is undefined behaviour. 
	template<typename... T>
	class ComponentView
	{
	public:
		using ComponentList = TypeList<T...>;
		using ComponentTuple  = typename ComponentList::TupleType;
		using PoolTuple = std::tuple<Ref<ComponentPool<T>>...>;

	public:
		ComponentView(const PoolTuple& pools)
			: mPools(pools)
		{
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
			LAB_STATIC_ASSERT(ComponentList::Contains<U>, "Template argument is not a component type in this view!");
			LAB_CORE_ASSERT(Contains(entity), "View does not contain this entity!");

			Ref<ComponentPool<U>> pool = std::get<ComponentList::Index<U>>(mPools);
			return pool->get(entity);
		}

		usize size() const { return mValidEntities.size(); }

		auto begin() { return mValidEntities.begin(); }
		auto begin() const { return mValidEntities.cbegin(); }
		auto end() { return mValidEntities.end(); }
		auto end() const { return mValidEntities.cend(); }

	private:
		bool Contains(EntityID entity) const
		{
			return std::find(mValidEntities.begin(), mValidEntities.end(), entity) != mValidEntities.end();
		}

		void BuildEntityList()
		{
			mValidEntities.clear();
			Ref<IComponentPool> smallestPool = GetSmallest();

			for (EntityID entity : smallestPool)
			{
				if (ValidEntity(entity))
					mValidEntities.push_back(entity);
			}
		}

		Ref<IComponentPool> GetSmallestImpl(Ref<ComponentPool<T>>... pools)
		{
			usize finalPoolSize = -1;
			Ref<IComponentPool> pool = nullptr;
			([&, this]
			{
				usize poolSize = pools->size();
				if (poolSize < finalPoolSize)
				{
					pool = pools;
					finalPoolSize = poolSize;
				}
			} (), ...);

			return pool;
		}
		Ref<IComponentPool> GetSmallest()
		{
			return std::apply(&ComponentView<T...>::GetSmallestImpl, std::tuple_cat(std::make_tuple(this), mPools));
		}

		bool ValidEntityImpl(Ref<ComponentPool<T>>... pools)
		{
			bool exists = true;
			([&, this]
			{
				if (!pools->exists(mCheckEntity))
					exists = false;
			} (), ...);

			return exists;
		}
		bool ValidEntity(EntityID entity)
		{
			mCheckEntity = entity;
			return std::apply(&ComponentView<T...>::ValidEntityImpl, std::tuple_cat(std::make_tuple(this), mPools));
		}

		template<usize index>
		auto& ToTupleElement(EntityID entity)
		{
			using ElementType = typename ComponentList::Type<index>;
			Ref<ComponentPool<ElementType>> pool = std::get<index>(mPools);
			return pool->get(entity);
		}

		template <usize... Is>
		auto GenTupleImpl(EntityID entity, std::index_sequence<Is...>) { return std::tie(ToTupleElement<Is>(entity)...); }

		auto GenTuple(EntityID entity) { return GenTupleImpl(entity, std::make_index_sequence<ComponentList::Size>{}); }

	private:
		PoolTuple mPools;
		std::vector<EntityID> mValidEntities;
		EntityID mCheckEntity;
	};

}