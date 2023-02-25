#pragma once

#include "Components.h"
#include "Scene.h"

namespace Laby::ECS {

	template<IsComponent TOwned, IsComponent... TOther>
	struct Group
	{
		using Type = entt::basic_group<entt::entity, entt::owned_t<TOwned>, entt::get_t<TOther...>, entt::exclude_t<>>;
		Type group;

		Group() = default;
		Group(Ref<Scene> context)
			: group(context->group<TOwned, TOther...>()) {}

		template<typename F>
		void each(F func) { group.each(func); }
	};

	using Animation = Group<AnimationComponent, SpriteRendererComponent>;

	using AllGroups = TypeList<Animation>;

	template<typename T>
	concept IsGroup = AllGroups::Contains<T>;

	class Groups
	{
	public:
		Groups(Ref<Scene> context);

		template<IsGroup T, typename F>
		void each(F func) { std::get<T>(*mGroupTuple).each(func); }

	private:
		template<usize Index>
		auto ToTupleElement(Ref<Scene> context)
		{
			using ElementType = typename AllGroups::Type<Index>;
			return ElementType(context);
		}

		template <usize... Is>
		auto GenerateTupleImpl(Ref<Scene> context, std::index_sequence<Is...>) { return std::make_tuple(ToTupleElement<Is>(context)...); }

		auto GenerateTuple(Ref<Scene> context) { return GenerateTupleImpl(context, std::make_index_sequence<AllGroups::Size>{}); }

	private:
		Single<AllGroups::TupleType> mGroupTuple;
	};
}