#include "Lpch.h"
#include "ScriptClass.h"

#include "ScriptEngineInternal.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

namespace Labyrinth {

	ScriptClass::ScriptClass(const std::string& classNamespace, const std::string& className)
		: mClassNamespace(classNamespace), mClassName(className)
	{
		mMonoClass = mono_class_from_name(ScriptEngineInternal::GetAppAssemblyInfo()->assemblyImage, classNamespace.c_str(), className.c_str());
	}

	ScriptClass::ScriptClass(MonoClass* klass)
		: mClassNamespace(mono_class_get_namespace(klass)), mClassName(mono_class_get_name(klass)), mMonoClass(klass)
	{
	}

	ScriptClass::ScriptClass(MonoObject* instance)
	{
		if (!instance) return;

		mMonoClass = mono_object_get_class(instance);
		mClassNamespace = mono_class_get_namespace(mMonoClass);
		mClassName = mono_class_get_name(mMonoClass);
	}

	MonoObject* ScriptClass::instantiate() const
	{
		return ScriptUtils::InstantiateClass(ScriptEngineInternal::GetAppDomain(), mMonoClass);
	}

	MonoMethod* ScriptClass::getMethod(const std::string& name, int argc)
	{
		return mono_class_get_method_from_name(mMonoClass, name.c_str(), argc);
	}

	MonoObject* ScriptClass::InstantiateInternal(void** argv, int argc)
	{
		MonoMethod* constructor = ScriptEngineInternal::GetCoreEntityClass()->getMethod(".ctor", 1);
		return ScriptUtils::InstantiateClassInternal(ScriptEngineInternal::GetAppDomain(), mMonoClass, constructor, argv);
	}

}