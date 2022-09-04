#pragma once

#include "ScriptUtils.h"
#include "ScriptCache.h"

#include <Labyrinth/Core/System/Ref.h>

namespace Labyrinth {

	class ScriptObject : public RefCounted
	{
	public:
		ScriptObject(std::nullptr_t) {}
		ScriptObject(MonoObject* instance)
			: mClass(ScriptClass::Create(instance)), mInstance(instance)
		{
			mOnStartMethod = mClass->getMethod("OnCreate", 0);
			mOnUpdateMethod = mClass->getMethod("OnUpdate", 1);
		}

		ScriptObject(const Ref<ScriptClass>& type)
			: mClass(type)
		{
			mInstance = mClass->instantiate();

			mOnStartMethod = mClass->getMethod("OnCreate", 0);
			mOnUpdateMethod = mClass->getMethod("OnUpdate", 1);
		}
		template<typename... Args>
		ScriptObject(const Ref<ScriptClass>& type, Args&&... args)
			: mClass(type)
		{
			mInstance = mClass->instantiate(std::forward<Args>(args)...);

			mOnStartMethod = mClass->getMethod("OnCreate", 0);
			mOnUpdateMethod = mClass->getMethod("OnUpdate", 1);
		}

		void onStart() { ScriptUtils::CallMethod(mInstance, mOnStartMethod); }
		void onUpdate(float ts) { ScriptUtils::CallMethod(mInstance, mOnUpdateMethod, ts); }

		bool valid() const { return mInstance; }
		MonoObject* obj() { return mInstance; }

		Ref<ScriptClass> getScriptClass() const { return mClass; }

		template<typename... Args>
		ScriptObject invokeMethod(const std::string& name, Args&&... args)
		{
			MonoObject* result = mClass->invokeMethod(mInstance, name, std::forward<Args>(args)...);

			// If no return value return nullptr literal so std::nullptr_t constructor is called for ScriptObject
			if (!result) return nullptr;
			return result;
		}

		template<typename T>
		void getFieldValue(const std::string& fieldName, T& result) const
		{
			const ScriptField* scriptField = mClass->getField(fieldName);
			if (!scriptField)
				return;

			LAB_CORE_ASSERT(ScriptFieldTypes::IsValidType<T>(scriptField->type), "Template type is not valid for this field type!");
			GetFieldValueInternal(scriptField, &result);
		}
		template<typename T>
		T getFieldValue(const std::string& fieldName) const
		{
			const ScriptField* scriptField = mClass->getField(fieldName);
			if (!scriptField)
				return;

			LAB_CORE_ASSERT(ScriptFieldTypes::IsValidType<T>(scriptField->type), "Template type is not valid for this field type!");

			T result = T();
			GetFieldValueInternal(scriptField, &result);

			return result;
		}
		void getEntityFieldValue(const std::string& fieldName, UUID& result) const;

		template<typename T>
		void setFieldValue(const std::string& fieldName, T& fieldVal)
		{
			const ScriptField* scriptField = mClass->getField(fieldName);
			if (!scriptField)
				return;

			LAB_CORE_ASSERT(ScriptFieldTypes::IsValidType<T>(scriptField->type), "Value is not a valid type for this field!");

			SetFieldValueInternal(scriptField, &fieldVal);
		}

		void setFieldValues(const FieldValues& fieldVals);

		template<typename... Args>
		static Ref<ScriptObject> Create(Args&&... args) { return Ref<ScriptObject>::Create(std::forward<Args>(args)...); }

	private:
		void GetFieldValueInternal(const ScriptField* scriptField, void* result) const;
		void SetFieldValueInternal(const ScriptField* scriptField, void* fieldVal);

	private:
		Ref<ScriptClass> mClass = nullptr;
		MonoObject* mInstance = nullptr;

		MonoMethod* mOnStartMethod = nullptr;
		MonoMethod* mOnUpdateMethod = nullptr;
	};
}