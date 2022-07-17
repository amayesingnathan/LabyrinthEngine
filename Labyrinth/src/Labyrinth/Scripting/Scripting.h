#pragma once

#include "ScriptingFwd.h"

#include <string>

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

	namespace Scripting {

		char* ReadBytes(const std::string& filepath, uint32_t* outSize);
		MonoAssembly* LoadCSharpAssembly(const std::string& assemblyPath);

		void PrintAssemblyTypes(MonoAssembly* assembly);
		MonoClass* GetClassInAssembly(MonoAssembly* assembly, const char* namespaceName, const char* className);
		MonoObject* InstantiateClass(MonoDomain* domain, MonoAssembly* assembly, const char* namespaceName, const char* className);

		void CallMethodInternal(MonoObject* instance, const char* methodName, void** argv, int argc);

		template<typename... Args>
		void CallMethod(MonoObject* instance, const char* methodName, Args&&... args)
		{
			constexpr size_t argc = sizeof...(Args);

			if (argc == 0)
				CallMethodInternal(instance, methodName, nullptr, 0);

			void* voidArgs[argc] = { nullptr };

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
					voidArgs[i] = tmp;
				}

				i++;
			}(), ...);

			CallMethodInternal(instance, methodName, voidArgs, argc);
		}

		uint8_t GetFieldAccessibility(MonoClassField* field);
		uint8_t GetPropertyAccessibility(MonoProperty* property);

		bool CheckMonoError(MonoError& error);

	}
}