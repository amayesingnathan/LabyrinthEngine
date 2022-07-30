#pragma once

#include "ScriptFwd.h"

#include <string>
#include <filesystem>

namespace Labyrinth {

	namespace Accessibility
	{
		enum : uint8_t
		{
			None = 0,
			Private = (1 << 0),
			Internal = (1 << 1),
			Protected = (1 << 2),
			Public = (1 << 3)
		};
	}

	class ScriptUtils
	{
	public:
		template<typename T, typename Func, typename... Args>
		static T WrapArgs(Func function, Args&&... args)
		{
			constexpr size_t argc = sizeof...(Args);
			
			if constexpr (argc != 0)
			{
				const void* argv[argc] = { nullptr };

				size_t i = 0;
				([&]()
					{
						const void* tmp = nullptr;
						if constexpr (IsPointer<Args>())
							tmp = args;
						else
							tmp = &args;
						argv[i] = tmp;

						i++;
					}(), ...);

				return function((void**)argv, Cast<int>(argc));
			}
			else return function(nullptr, 0);
		}

		static MonoAssembly* LoadMonoAssembly(const std::filesystem::path& assemblyPath);

		static void PrintAssemblyTypes(MonoAssembly* assembly);

		static MonoClass* GetClassInAssembly(MonoAssembly* assembly, const char* namespaceName, const char* className);

		static MonoObject* InstantiateClass(MonoDomain* domain, MonoClass* classInstance);
		template<typename... Args>
		static MonoObject* InstantiateClass(MonoDomain* domain, MonoClass* classInstance, Args&&... args)
		{
			return ScriptUtils::WrapArgs<MonoObject*>([=](void** argv, int argc) {
				return InstantiateClassInternal(domain, classInstance, argv, argc);
				}, std::forward<Args>(args)...);
		}

		template<typename... Args>
		static MonoObject* CallMethod(MonoObject* instance, const char* methodName, Args&&... args)
		{
			return ScriptUtils::WrapArgs<MonoObject*>([=](void** argv, int argc) {
				return CallMethodInternal(instance, methodName, argv, argc);
				}, std::forward<Args>(args)...);
		}
		template<typename... Args>
		static MonoObject* CallMethod(MonoObject* instance, MonoMethod* method, Args&&... args)
		{
			return ScriptUtils::WrapArgs<MonoObject*>([=](void** argv, int argc) {
				return CallMethodInternal(instance, method, argv);
				}, std::forward<Args>(args)...);
		}

	private:
		static bool CheckMonoError(MonoError& error);

		static MonoMethod* GetMethodInternal(MonoClass* classInstance, const char* methodName, int argc);
		static MonoObject* CallMethodInternal(MonoObject* instance, const char* methodName, void** argv, int argc);
		static MonoObject* CallMethodInternal(MonoObject* instance, MonoMethod* method, void** argv);
		static MonoObject* InstantiateClassInternal(MonoDomain* domain, MonoClass* classInstance, void** argv, int argc);
		static MonoObject* InstantiateClassInternal(MonoDomain* domain, MonoClass* classInstance, MonoMethod* constructor, void** argv);

		friend class ScriptClass;
	};
}