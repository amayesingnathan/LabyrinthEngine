#pragma once

#include "ScriptFwd.h"

#include <Labyrinth/Core/System/Ref.h>

#include <string>
#include <unordered_map>

namespace Labyrinth {

	class ScriptClass : public RefCounted
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

		MonoMethod* getMethod(const std::string& name, int argc);

		template<typename... Args>
		MonoObject* invokeMethod(MonoObject* instance, const std::string& name, Args&&... args)
		{
			return ScriptUtils::CallMethod(instance, getMethod(name), std::forward<Args>(args)...);
		}

		operator MonoClass* () { return mMonoClass; }

		static Ref<ScriptClass> Create(const std::string& classNamespace, const std::string& className) { return Ref<ScriptClass>::Create(classNamespace, className); }
		static Ref<ScriptClass> Create(MonoClass* klass) { return Ref<ScriptClass>::Create(klass); }
		static Ref<ScriptClass> Create(MonoObject* instance) { return Ref<ScriptClass>::Create(instance); }

	private:
		MonoObject* InstantiateInternal(void** argv, int argc);

	private:
		std::string mClassNamespace;
		std::string mClassName;

		MonoClass* mMonoClass = nullptr;
	};

}