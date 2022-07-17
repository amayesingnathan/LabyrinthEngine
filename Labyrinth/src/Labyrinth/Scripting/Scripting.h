#pragma once

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/object.h>
#include <mono/metadata/attrdefs.h>

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

	class Scripting
	{
	public:
		static MonoAssembly* LoadCSharpAssembly(const std::string& assemblyPath);

		static void PrintAssemblyTypes(MonoAssembly* assembly);
		static MonoClass* GetClassInAssembly(MonoAssembly* assembly, const char* namespaceName, const char* className);
		static MonoObject* InstantiateClass(MonoDomain* domain, MonoAssembly* assembly, const char* namespaceName, const char* className);

		template<typename... Args>
		static void CallMethod(MonoObject* instance, const char* methodName, Args&&... args)
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

		template<typename T>
		static void GetField(MonoObject* instance, const char* fieldName, T& data)
		{
			void* voidData = data;
			GetFieldInternal(instance, fieldName, &voidData);
			LAB_CORE_ASSERT(voidData, "Field return value from Mono was null!");
			data = *voidData;
		}

		template<typename T>
		static T GetProperty(MonoObject* instance, const char* propertyName)
		{
			return *Cast<T>(GetPropertyInternal(instance, propertyName));
		}
		static MonoObject* GetPropertyRef(MonoObject* instance, const char* propertyName);

		static void SetField(MonoObject* instance, const char* fieldName, void* data);
		static void SetProperty(MonoObject* instance, const char* propertyName, void* data);

	private:
		static char* ReadBytes(const std::string& filepath, uint32_t* outSize);

		static bool CheckMonoError(MonoError& error);

		static void CallMethodInternal(MonoObject* instance, const char* methodName, void** argv, int argc);

		static uint8_t GetFieldAccessibility(MonoClassField* field);
		static uint8_t GetPropertyAccessibility(MonoProperty* property);

		static void GetFieldInternal(MonoObject* instance, const char* fieldName, void* data);
		static void* GetPropertyInternal(MonoObject* instance, const char* propertyName);

	};
}