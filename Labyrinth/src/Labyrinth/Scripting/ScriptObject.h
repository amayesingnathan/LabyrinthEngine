#pragma once

#include "ScriptClass.h"
#include "ScriptField.h"

#include <Labyrinth/Core/System/Ref.h>
#include <Labyrinth/Core/UUID.h>

namespace Laby {

	class ScriptObject : public RefCounted
	{
	public:
		ScriptObject(std::nullptr_t) {}
		ScriptObject(MonoObject* instance)
			: mClass(Ref<ScriptClass>::Create(instance)), mInstance(instance)
		{
			mOnStartMethod = mClass->getMethod("OnCreate", 0);
			mOnUpdateMethod = mClass->getMethod("OnUpdate", 1);
			mOnCollisionBeginMethod = mClass->getMethod("OnCollisionBegin", 1);
			mOnCollisionEndMethod = mClass->getMethod("OnCollisionEnd", 1);
		}

		ScriptObject(const Ref<ScriptClass>& type)
			: mClass(type)
		{
			mInstance = mClass->instantiate();

			mOnStartMethod = mClass->getMethod("OnCreate", 0);
			mOnUpdateMethod = mClass->getMethod("OnUpdate", 1);
			mOnCollisionBeginMethod = mClass->getMethod("OnCollisionBegin", 1);
			mOnCollisionEndMethod = mClass->getMethod("OnCollisionEnd", 1);
		}
		template<typename... Args>
		ScriptObject(const Ref<ScriptClass>& type, Args&&... args)
			: mClass(type)
		{
			mInstance = mClass->instantiate(std::forward<Args>(args)...);

			mOnStartMethod = mClass->getMethod("OnCreate", 0);
			mOnUpdateMethod = mClass->getMethod("OnUpdate", 1);
			mOnCollisionBeginMethod = mClass->getMethod("OnCollisionBegin", 1);
			mOnCollisionEndMethod = mClass->getMethod("OnCollisionEnd", 1);
		}

		void onStart() const { ScriptUtils::CallMethod(mInstance, mOnStartMethod); }
		void onUpdate(float ts) const { ScriptUtils::CallMethod(mInstance, mOnUpdateMethod, ts); }
		void onCollisionBegin(Entity entity) const { ScriptUtils::CallMethod(mInstance, mOnCollisionBeginMethod, entity.getUUID()); }
		void onCollisionEnd(Entity entity) const { ScriptUtils::CallMethod(mInstance, mOnCollisionEndMethod, entity.getUUID()); }

		bool valid() const { return mInstance; }
		MonoObject* obj() { return mInstance; }

		Ref<ScriptClass> getScriptClass() const { return mClass; }

		template<typename... Args>
		ScriptObject invokeMethod(const std::string& name, Args&&... args) const
		{
			MonoObject* result = mClass->invokeMethod(mInstance, name, std::forward<Args>(args)...);

			// If no return value return nullptr literal so std::nullptr_t constructor is called for ScriptObject
			if (!result)
				return nullptr;
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

	private:
		void GetFieldValueInternal(const ScriptField* scriptField, void* result) const;
		void SetFieldValueInternal(const ScriptField* scriptField, void* fieldVal);

	private:
		Ref<ScriptClass> mClass = nullptr;
		MonoObject* mInstance = nullptr;

		MonoMethod* mOnStartMethod = nullptr;
		MonoMethod* mOnUpdateMethod = nullptr;
		MonoMethod* mOnCollisionBeginMethod = nullptr; 
		MonoMethod* mOnCollisionEndMethod = nullptr;
	};
}