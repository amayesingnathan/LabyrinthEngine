#pragma once

#include "Scripting.h"

namespace Labyrinth {

	class ScriptEngine
	{
	public:
		static void Init();
		static void Shutdown();

		static void LoadCoreAssembly(const std::filesystem::path& path);

	private:
		static void InitMono();
		static void ShutdownMono();
	};

	class ScriptClass
	{
	public:
		ScriptClass() = default;
		ScriptClass(const std::string& classNamespace, const std::string& className);
		
		MonoObject* instantiate();
		template<typename... Args>
		MonoObject* instantiate(Args&&... args)
		{
			MonoObject* instance = nullptr;
			Scripting::WrapArgs([=](void** argv, size_t argc) {
				return InstantiateInternal(argv, argc);
				}, instance, std::forward<Args>(args)...);
			return instance;
		}

		MonoMethod* getMethod(const std::string& name, size_t argc);

		template<typename... Args>
		MonoObject* invokeMethod(MonoObject* instance, const std::string& name, Args&&... args)
		{
			return Scripting::CallMethod(instance, name.c_str(), std::forward<Args>(args)...);
		}

	private:
		MonoObject* InstantiateInternal(void** argv, size_t argc);

	private:
		std::string mClassNamespace;
		std::string mClassName;

		MonoClass* mMonoClass = nullptr;
	};


}