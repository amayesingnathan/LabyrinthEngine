#pragma once

#include "ScriptDefinitions.h"

#include <Labyrinth/Core/System/Base.h>

#include <string>
#include <filesystem>

namespace Laby {

	class ScriptUtils
	{
	public:
		template<typename T, typename Func, typename... Args>
		static T WrapArgs(Func&& function, Args&&... args)
		{
			constexpr size_t argc = sizeof...(Args);

			if constexpr (argc != 0)
			{
				const void* argv[argc] = { nullptr };

				size_t i = 0;
				([&]()
				{
					const void* tmp = nullptr;
					if constexpr (TypeTraits<Args>::IsPointer)
						tmp = args;
					else
						tmp = &args;
					argv[i] = tmp;

					i++;
				}(), ...);

				return function((void**)argv, StaticCast<int>(argc));
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

		static bool CheckMonoError(MonoError& error);

		static u8 GetFieldAccessibility(MonoClassField* field);

	private:
		static MonoMethod* GetMethodInternal(MonoClass* classInstance, const char* methodName, int argc);
		static MonoObject* CallMethodInternal(MonoObject* instance, const char* methodName, void** argv, int argc);
		static MonoObject* CallMethodInternal(MonoObject* instance, MonoMethod* method, void** argv);
		static MonoObject* InstantiateClassInternal(MonoDomain* domain, MonoClass* classInstance, void** argv, int argc);
		static MonoObject* InstantiateClassInternal(MonoDomain* domain, MonoClass* classInstance, MonoMethod* constructor, void** argv);

		friend class ScriptClass;
	};
}