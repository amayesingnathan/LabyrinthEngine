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

	class Scripting
	{
	public:
		template<typename Func, typename T, typename... Args>
		static void WrapArgs(Func function, T* result, Args&&... args)
		{
			constexpr size_t argc = sizeof...(Args);

			if constexpr (argc == 0)
			{
				function(nullptr, 0);
			}
			else
			{
				void* argv[argc] = { nullptr };

				size_t i = 0;
				([&]()
					{
						if (i < argc)
						{
							void* tmp = nullptr;
							if constexpr (IsPointer<Args>::value)
								tmp = args;
							else
								tmp = &args;
							argv[i] = tmp;
						}

						i++;
					}(), ...);

				result = function(argv, argc);
			}
		}

		static MonoAssembly* LoadMonoAssembly(const std::filesystem::path& assemblyPath);

		static void PrintAssemblyTypes(MonoAssembly* assembly);

		static MonoClass* GetClassInAssembly(MonoAssembly* assembly, const char* namespaceName, const char* className);

		static MonoObject* InstantiateClass(MonoDomain* domain, MonoClass* classInstance);
		template<typename... Args>
		static MonoObject* InstantiateClass(MonoDomain* domain, MonoClass* classInstance, Args&&... args)
		{
			MonoObject* instance = nullptr;
			Scripting::WrapArgs([=](void** argv, size_t argc) {
				return InstantiateClassInternal(domain, classInstance, argv, argc);
				}, instance, std::forward<Args>(args)...);
			return instance;
		}

		template<typename... Args>
		static MonoObject* CallMethod(MonoObject* instance, const char* methodName, Args&&... args)
		{
			MonoObject* returnInstance = nullptr;
			Scripting::WrapArgs([=](void** argv, size_t argc) {
				return CallMethodInternal(instance, methodName, argv, argc);
				}, returnInstance, std::forward<Args>(args)...);
			return returnInstance;
		}

	private:
		static bool CheckMonoError(MonoError& error);

		static MonoMethod* GetMethodInternal(MonoClass* classInstance, const char* methodName, size_t argc);
		static MonoObject* CallMethodInternal(MonoObject* instance, const char* methodName, void** argv, int argc);
		static MonoObject* InstantiateClassInternal(MonoDomain* domain, MonoClass* classInstance, void** argv, size_t argc);


		friend class ScriptClass;
	};
}