#pragma once

#include "ScriptUtils.h"
#include "ScriptClass.h"

namespace Labyrinth {

	class ScriptObject
	{
	public:
		ScriptObject(std::nullptr_t) {}
		ScriptObject::ScriptObject(MonoObject* instance)
			: mClass(ScriptClass::Create(instance)), mInstance(instance)
		{
		}

		ScriptObject(const Ref<ScriptClass>& type)
		: mClass(type)
		{
			mInstance = mClass->instantiate();
		}
		template<typename... Args>
		ScriptObject(const Ref<ScriptClass>& type, Args&&... args)
			: mClass(type)
		{
			mInstance = mClass->instantiate(std::forward<Args>(args)...);
		}

		template<typename... Args>
		ScriptObject invokeMethod(const std::string& name, Args&&... args)
		{
			MonoObject* result = ScriptUtils::CallMethod(mInstance, mClass->getMethod(name), std::forward<Args>(args)...);
			if (result) return result;

			return nullptr;
		}

	private:
		Ref<ScriptClass> mClass = nullptr;
		MonoObject* mInstance = nullptr;
	};

}