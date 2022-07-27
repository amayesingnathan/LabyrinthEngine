#pragma once

#include "ScriptFwd.h"

#include <string>
#include <unordered_map>

namespace Labyrinth {

	class ScriptClass
	{
	public:
		ScriptClass() = default;
		ScriptClass(const std::string& classNamespace, const std::string& className);
		ScriptClass(MonoClass* klass);
		ScriptClass(MonoObject* instance);

		MonoObject* instantiate() const;
		template<typename... Args>
		MonoObject* instantiate(Args&&... args)
		{
			return ScriptUtils::WrapArgs<MonoObject*>([=](void** argv, int argc) {
				return InstantiateInternal(argv, argc);
				}, std::forward<Args>(args)...);
		}

		MonoMethod* getMethod(const std::string& name);

		template<typename... Args>
		MonoObject* invokeMethod(MonoObject* instance, const std::string& name, Args&&... args)
		{
			return ScriptUtils::CallMethod(instance, getMethod(name), std::forward<Args>(args)...);
		}

		operator MonoClass* () { return mMonoClass; }

		static Ref<ScriptClass> Create(const std::string& classNamespace, const std::string& className) { return CreateRef<ScriptClass>(classNamespace, className); }
		static Ref<ScriptClass> Create(MonoClass* klass) { return CreateRef<ScriptClass>(klass); }
		static Ref<ScriptClass> Create(MonoObject* instance) { return CreateRef<ScriptClass>(instance); }

	private:
		MonoObject* InstantiateInternal(void** argv, int argc);

		void LoadMethods();

	private:
		std::string mClassNamespace;
		std::string mClassName;

		MonoClass* mMonoClass = nullptr;
		std::unordered_map<std::string, MonoMethod*> mMethods;
	};

}