#pragma once

#include "Scripting.h"
#include "ScriptClass.h"

namespace Labyrinth {

	class ScriptObject
	{
	public:
		ScriptObject::ScriptObject(MonoObject* instance)
			: mClass(ScriptClass::Create(instance)), mInstance(instance) {}

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
			return Scripting::CallMethod(mInstance, mClass->getMethod(name), std::forward<Args>(args)...);
		}

	private:
		Ref<ScriptClass> mClass;
		MonoObject* mInstance = nullptr;
	};

}