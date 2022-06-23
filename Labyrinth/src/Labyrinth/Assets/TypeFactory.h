#pragma once

#include <string>
#include <unordered_map>
#include <functional>

#include <Labyrinth/Core/System/Assert.h>

namespace Labyrinth {

	// Func is a lambda that should return a Ref<Type>
	template<typename Type, typename Func>
	class TypeFactory
	{
	private:
		TypeFactory() = default;
		TypeFactory(const TypeFactory&) = delete;
		void operator=(const TypeFactory&) = delete;

	private:
		using ObjectMap = std::unordered_map<std::string, Func>;
		ObjectMap mTypes;

	private:
		static ObjectMap& Get()
		{
			static TypeFactory<Type, Func> mFactory;
			return mFactory.mTypes;
		}

	public:
		template<typename T>
		static void Register(const std::string& key, Func& f)
		{
			ObjectMap& data = Get();
			if (data.count(key) != 0)) return;

			data.emplace(key, f);
		}

		Ref<Type> Create(const std::string key)
		{
			ObjectMap& data = Get();
			LAB_CORE_ASSERT(data.count(key) != 0, "Type is not registered: {0}", key);

			return data[key]();
		}

		template<typename... Args>
		Ref<Type> Create(const std::string key)
		{
			ObjectMap& data = Get();
			LAB_CORE_ASSERT(data.count(key) != 0, "Type is not registered: {0}", key);

			return data[key](std::forward<Args>(args)...);
		}
	};
}