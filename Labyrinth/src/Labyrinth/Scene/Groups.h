#pragma once

#include "Components.h"
#include "Scene.h"

namespace Laby::ECS {

	template<IsComponent TOwned, IsComponent... TOther>
	struct Group
	{
		using Type = entt::basic_group<entt::entity, entt::owned_t<TOwned>, entt::get_t<TOther...>, entt::exclude_t<>>;
		Type group;

		Group(Scene* context)
			: group(context->group<TOwned, TOther...>()) {}

		template<typename F>
		void each(F func) { group.each(func); }
	};

	using Animations = Group<AnimationComponent, SpriteRendererComponent>;
	using Bodies = Group<RigidBodyComponent, TransformComponent>;
	using Circles = Group<CircleRendererComponent, TransformComponent>;
	using Sprites = Group<SpriteRendererComponent, TransformComponent>;

	using AllGroups = TypeList<Animations, Bodies, Circles, Sprites>;

	template<typename T>
	concept IsGroup = AllGroups::Contains<T>;

	class Groups
	{
	public:
		Groups(Scene* context);

		template<IsGroup T, typename F>
		void each(F func) { std::get<T>(mGroupTuple).each(func); }

	private:
		template<usize Index>
		auto ToTupleElement(Scene* context)
		{
			using ElementType = typename AllGroups::Type<Index>;
			return ElementType(context);
		}

		template <usize... Is>
		AllGroups::TupleType GenerateTupleImpl(Scene* context, std::index_sequence<Is...>) { return std::make_tuple(ToTupleElement<Is>(context)...); }

		AllGroups::TupleType GenerateTuple(Scene* context) { return GenerateTupleImpl(context, std::make_index_sequence<AllGroups::Size>{}); }

	private:
		AllGroups::TupleType mGroupTuple;
	};
}