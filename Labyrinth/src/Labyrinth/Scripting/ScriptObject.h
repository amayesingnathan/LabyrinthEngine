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
			mOnStartMethod = mClass->GetMethod("OnCreate", 0);
			mOnUpdateMethod = mClass->GetMethod("OnUpdate", 1);
			mOnCollisionBeginMethod = mClass->GetMethod("OnCollisionBegin", 1);
			mOnCollisionEndMethod = mClass->GetMethod("OnCollisionEnd", 1);
		}

		ScriptObject(const Ref<ScriptClass>& type)
			: mClass(type)
		{
			mInstance = mClass->Instantiate();

			mOnStartMethod = mClass->GetMethod("OnCreate", 0);
			mOnUpdateMethod = mClass->GetMethod("OnUpdate", 1);
			mOnCollisionBeginMethod = mClass->GetMethod("OnCollisionBegin", 1);
			mOnCollisionEndMethod = mClass->GetMethod("OnCollisionEnd", 1);
		}
		template<typename... Args>
		ScriptObject(const Ref<ScriptClass>& type, Args&&... args)
			: mClass(type)
		{
			mInstance = mClass->Instantiate(std::forward<Args>(args)...);

			mOnStartMethod = mClass->GetMethod("OnCreate", 0);
			mOnUpdateMethod = mClass->GetMethod("OnUpdate", 1);
			mOnCollisionBeginMethod = mClass->GetMethod("OnCollisionBegin", 1);
			mOnCollisionEndMethod = mClass->GetMethod("OnCollisionEnd", 1);
		}

		void OnStart() const { ScriptUtils::CallMethod(mInstance, mOnStartMethod); }
		void OnUpdate(float ts) const { ScriptUtils::CallMethod(mInstance, mOnUpdateMethod, ts); }
		void OnCollisionBegin(Entity entity) const { ScriptUtils::CallMethod(mInstance, mOnCollisionBeginMethod, entity.GetUUID()); }
		void OnCollisionEnd(Entity entity) const { ScriptUtils::CallMethod(mInstance, mOnCollisionEndMethod, entity.GetUUID()); }

		bool valid() const { return mInstance; }
		MonoObject* obj() { return mInstance; }

		Ref<ScriptClass> GetScriptClass() const { return mClass; }

		template<typename... Args>
		ScriptObject InvokeMethod(std::string_view name, Args&&... args) const
		{
			MonoObject* result = mClass->InvokeMethod(mInstance, name, std::forward<Args>(args)...);

			// If no return value return nullptr literal so std::nullptr_t constructor is called for ScriptObject
			if (!result)
				return nullptr;
			return result;
		}

		template<typename T>
		void GetFieldValue(std::string_view fieldName, T& result) const
		{
			const ScriptField* scriptField = mClass->GetField(fieldName);
			if (!scriptField)
				return;

			LAB_CORE_ASSERT(ScriptFieldTypes::IsValidType<T>(scriptField->type), "Template type is not valid for this field type!");
			GetFieldValueInternal(scriptField, &result);
		}
		template<typename T>
		T GetFieldValue(std::string_view fieldName) const
		{
			const ScriptField* scriptField = mClass->GetField(fieldName);
			if (!scriptField)
				return;

			LAB_CORE_ASSERT(ScriptFieldTypes::IsValidType<T>(scriptField->type), "Template type is not valid for this field type!");

			T result = T();
			GetFieldValueInternal(scriptField, &result);

			return result;
		}
		void GetEntityFieldValue(std::string_view fieldName, UUID& result) const;

		template<typename T>
		void SetFieldValue(std::string_view fieldName, const T& fieldVal)
		{
			const ScriptField* scriptField = mClass->GetField(fieldName);
			if (!scriptField)
				return;

			LAB_CORE_ASSERT(ScriptFieldTypes::IsValidType<T>(scriptField->type), "Value is not a valid type for this field!");

			SetFieldValueInternal(scriptField, &fieldVal);
		}

		void SetFieldValues(const FieldValues& fieldVals);

	private:
		void GetFieldValueInternal(const ScriptField* scriptField, void* result) const;
		void SetFieldValueInternal(const ScriptField* scriptField, const void* fieldVal);

	private:
		Ref<ScriptClass> mClass = nullptr;
		MonoObject* mInstance = nullptr;

		MonoMethod* mOnStartMethod = nullptr;
		MonoMethod* mOnUpdateMethod = nullptr;
		MonoMethod* mOnCollisionBeginMethod = nullptr; 
		MonoMethod* mOnCollisionEndMethod = nullptr;
	};
}