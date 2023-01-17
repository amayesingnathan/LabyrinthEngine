#include "Lpch.h"
#include "ScriptClass.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

#include "ScriptUtils.h"
#include "ScriptEngineInternal.h"

namespace Laby {

	ScriptClass::ScriptClass(const std::string& classNamespace, const std::string& className)
		: mClassNamespace(classNamespace), mClassName(className)
	{
		mMonoClass = mono_class_from_name(ScriptEngineInternal::GetAppAssemblyInfo()->assemblyImage, classNamespace.c_str(), className.c_str());

		LoadFields();
	}

	ScriptClass::ScriptClass(MonoClass* klass)
		: mClassNamespace(mono_class_get_namespace(klass)), mClassName(mono_class_get_name(klass)), mMonoClass(klass)
	{
		LoadFields();
	}

	ScriptClass::ScriptClass(MonoObject* instance)
	{
		if (!instance) return;

		mMonoClass = mono_object_get_class(instance);
		mClassNamespace = mono_class_get_namespace(mMonoClass);
		mClassName = mono_class_get_name(mMonoClass);

		LoadFields();
	}

	MonoObject* ScriptClass::instantiate() const
	{
		return ScriptUtils::InstantiateClass(ScriptEngineInternal::GetAppDomain(), mMonoClass);
	}

	MonoMethod* ScriptClass::getMethod(const std::string& name, int argc)
	{
		return mono_class_get_method_from_name(mMonoClass, name.c_str(), argc);
	}

	const ScriptField* ScriptClass::getField(std::string_view name) const
	{
		auto it = std::find_if(mFields.begin(), mFields.end(), [&name](const ScriptField& field) { return field.name == name; });

		if (it == mFields.end())
			return nullptr;

		return &(*it);
	}

	void ScriptClass::LoadFields()
	{
		void* iterator = nullptr;
		while (MonoClassField* field = mono_class_get_fields(mMonoClass, &iterator))
		{
			if (!(ScriptUtils::GetFieldAccessibility(field) & Accessibility::Public))
				continue;

			std::string_view fieldTypeName = mono_type_get_name(mono_field_get_type(field));
			std::string_view fieldName = mono_field_get_name(field);
			mFields.emplace_back(ScriptFields::GetType(fieldTypeName), fieldName, field);
		}
	}

	MonoObject* ScriptClass::InstantiateInternal(void** argv, int argc)
	{
		MonoMethod* constructor = ScriptEngineInternal::GetCoreEntityClass()->getMethod(".ctor", 1);
		return ScriptUtils::InstantiateClassInternal(ScriptEngineInternal::GetAppDomain(), mMonoClass, constructor, argv);
	}

}