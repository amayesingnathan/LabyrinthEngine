#pragma once

#include <string>

struct _MonoDomain;
typedef struct _MonoDomain MonoDomain;
struct _MonoAssembly;
typedef struct _MonoAssembly MonoAssembly;
struct _MonoClass;
typedef struct _MonoClass MonoClass;
struct _MonoObject;
typedef struct _MonoObject MonoObject;

namespace Labyrinth {

	namespace Scripting {

		char* ReadBytes(const std::string& filepath, uint32_t* outSize);
		MonoAssembly* LoadCSharpAssembly(const std::string& assemblyPath);
		void PrintAssemblyTypes(MonoAssembly* assembly);
		MonoClass* GetClassInAssembly(MonoAssembly* assembly, const char* namespaceName, const char* className);

		MonoObject* InstantiateClass(MonoDomain* domain, MonoAssembly* assembly, const char* namespaceName, const char* className);

		void CallMethod(MonoObject* instance, const char* methodName, void** argv, int argc);

	}
}