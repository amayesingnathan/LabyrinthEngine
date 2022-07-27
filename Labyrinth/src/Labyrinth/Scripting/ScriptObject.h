#pragma once

#include "ScriptUtils.h"
#include "ScriptClass.h"

namespace Labyrinth {

	class ScriptObject
	{
	public:
		ScriptObject(std::nullptr_t) {}
		ScriptObject(MonoObject* instance)
			: mClass(ScriptClass::Create(instance)), mInstance(instance)
		{
			mOnStartMethod = mClass->getMethod("OnCreate");
			mOnUpdateMethod = mClass->getMethod("OnUpdate");
		}

		ScriptObject(const Ref<ScriptClass>& type)
			: mClass(type)
		{
			mInstance = mClass->instantiate();

			mOnStartMethod = mClass->getMethod("OnCreate");
			mOnUpdateMethod = mClass->getMethod("OnUpdate");
		}
		template<typename... Args>
		ScriptObject(const Ref<ScriptClass>& type, Args&&... args)
			: mClass(type)
		{
			mInstance = mClass->instantiate(std::forward<Args>(args)...);

			mOnStartMethod = mClass->getMethod("OnCreate");
			mOnUpdateMethod = mClass->getMethod("OnUpdate");
		}

		void onStart() { ScriptUtils::CallMethod(mInstance, mOnStartMethod); }
		void onUpdate(float ts) { ScriptUtils::CallMethod(mInstance, mOnUpdateMethod, ts); }

		template<typename... Args>
		ScriptObject invokeMethod(const std::string& name, Args&&... args)
		{
			MonoObject* result = mClass->invokeMethod(mInstance, name, std::forward<Args>(args)...);

			// If no return value return nullptr literal so std::nullptr_t constructor is called for ScriptObject
			if (!result) return nullptr;
			return result;
		}

		template<typename... Args>
		static Ref<ScriptObject> Create(Args&&... args) { return CreateRef<ScriptObject>(std::forward<Args>(args)...); }

	private:
		Ref<ScriptClass> mClass = nullptr;
		MonoObject* mInstance = nullptr;

		MonoMethod* mOnStartMethod = nullptr;
		MonoMethod* mOnUpdateMethod = nullptr;
	};

}