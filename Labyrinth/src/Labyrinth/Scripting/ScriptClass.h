#pragma once

#include "ScriptDefinitions.h"
#include "ScriptField.h"
#include "ScriptUtils.h"

#include <Labyrinth/Core/System/Ref.h>

#include <string>
#include <unordered_map>

namespace Laby {

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

		MonoClass* getClass() { return mMonoClass; }

		const ScriptField* getField(std::string_view name) const;
		const std::vector<ScriptField>& getFields() const { return mFields; }

		bool valid() { return mMonoClass; }

	private:
		void LoadFields();

		MonoObject* InstantiateInternal(void** argv, int argc);

	private:
		std::string mClassNamespace;
		std::string mClassName;

		MonoClass* mMonoClass = nullptr;

		std::vector<ScriptField> mFields;

		friend class ScriptEngineInternal;
	};

}