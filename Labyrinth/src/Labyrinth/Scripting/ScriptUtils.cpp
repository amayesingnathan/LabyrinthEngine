#include "Lpch.h"
#include "ScriptUtils.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/object.h>
#include <mono/metadata/attrdefs.h>

#include "ScriptField.h"

#include <Labyrinth/IO/Filesystem.h>

namespace Laby {

	MonoAssembly* ScriptUtils::LoadMonoAssembly(const std::filesystem::path& assemblyPath)
	{
		Buffer fileData = FileUtils::Read(assemblyPath);

		// NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
		MonoImageOpenStatus status;
		MonoImage* image = mono_image_open_from_data_full(fileData.as<char>(), (u32)fileData.size(), 1, &status, 0);

		if (status != MONO_IMAGE_OK)
		{
			const char* errorMessage = mono_image_strerror(status);
			LAB_CORE_ERROR(errorMessage);
			return nullptr;
		}

		MonoAssembly* assembly = mono_assembly_load_from_full(image, assemblyPath.string().c_str(), &status, 0);
		mono_image_close(image);

		return assembly;
	}

	void ScriptUtils::PrintAssemblyTypes(MonoAssembly* assembly)
	{
		MonoImage* image = mono_assembly_get_image(assembly);
		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

			LAB_CORE_TRACE("{}.{}", nameSpace, name);
		}
	}

	MonoClass* ScriptUtils::GetClassInAssembly(MonoAssembly* assembly, const char* namespaceName, const char* className)
	{
		MonoImage* image = mono_assembly_get_image(assembly);
		MonoClass* klass = mono_class_from_name(image, namespaceName, className);

		if (klass == nullptr)
		{
			LAB_CORE_ERROR("[ScriptEngine] The class could not be loaded!");
			return nullptr;
		}

		return klass;
	}

	MonoObject* ScriptUtils::InstantiateClass(MonoDomain* domain, MonoClass* classInstance)
	{
		// Allocate an instance of our class
		MonoObject* instance = mono_object_new(domain, classInstance);

		if (classInstance == nullptr)
		{
			LAB_CORE_ERROR("[ScriptEngine] The class could not be instantiated!");
			return nullptr;
		}

		mono_runtime_object_init(instance);

		return instance;
	}


	bool ScriptUtils::CheckMonoError(MonoError& error)
	{
		bool hasError = !mono_error_ok(&error);
		if (hasError)
		{
			unsigned short errorCode = mono_error_get_error_code(&error);
			const char* errorMessage = mono_error_get_message(&error);
			LAB_CORE_ERROR("Mono Error!");
			LAB_CORE_ERROR("\tError Code: {}", errorCode);
			LAB_CORE_ERROR("\tError Message: {}", errorMessage);
			mono_error_cleanup(&error);
		}
		return hasError;
	}

	u8 ScriptUtils::GetFieldAccessibility(MonoClassField* field)
	{
		u8 accessibility = (u8)Accessibility::None;
		u32 accessFlag = mono_field_get_flags(field) & MONO_FIELD_ATTR_FIELD_ACCESS_MASK;

		switch (accessFlag)
		{
		case MONO_FIELD_ATTR_PRIVATE:
		{
			accessibility = Accessibility::Private;
			break;
		}
		case MONO_FIELD_ATTR_FAM_AND_ASSEM:
		{
			accessibility |= Accessibility::Protected;
			accessibility |= Accessibility::Internal;
			break;
		}
		case MONO_FIELD_ATTR_ASSEMBLY:
		{
			accessibility = Accessibility::Internal;
			break;
		}
		case MONO_FIELD_ATTR_FAMILY:
		{
			accessibility = Accessibility::Protected;
			break;
		}
		case MONO_FIELD_ATTR_FAM_OR_ASSEM:
		{
			accessibility |= Accessibility::Private;
			accessibility |= Accessibility::Protected;
			break;
		}
		case MONO_FIELD_ATTR_PUBLIC:
		{
			accessibility = Accessibility::Public;
			break;
		}
		}

		return accessibility;
	}

	MonoMethod* ScriptUtils::GetMethodInternal(MonoClass* classInstance, const char* methodName, int argc)
	{
		return mono_class_get_method_from_name(classInstance, methodName, argc);;
	}

	MonoObject* ScriptUtils::CallMethodInternal(MonoObject* instance, const char* methodName, void** argv, int argc)
	{
		MonoMethod* method = GetMethodInternal(mono_object_get_class(instance), methodName, argc);

		if (!method)
		{
			LAB_CORE_ERROR("[ScriptEngine] Specified function with parameter count does not exist on object!");
			return nullptr;
		}

		return CallMethodInternal(instance, method, argv);
	}

	MonoObject* ScriptUtils::CallMethodInternal(MonoObject* instance, MonoMethod* method, void** argv)
	{
		MonoObject* exception = nullptr;
		MonoObject* result = mono_runtime_invoke(method, instance, argv, &exception);
		if (exception)
			mono_print_unhandled_exception(exception);

		return result;
	}

	MonoObject* ScriptUtils::InstantiateClassInternal(MonoDomain* domain, MonoClass* classInstance, void** argv, int argc)
	{
		// Allocate an instance of our class
		MonoObject* instance = mono_object_new(domain, classInstance);

		if (!classInstance)
		{
			LAB_CORE_ERROR("[ScriptEngine] The class could not be instantiated!");
			return nullptr;
		}

		MonoMethod* constructor = GetMethodInternal(classInstance, ".ctor", argc);

		mono_runtime_invoke(constructor, instance, argv, nullptr);

		return instance;
	}

	MonoObject* ScriptUtils::InstantiateClassInternal(MonoDomain* domain, MonoClass* classInstance, MonoMethod* constructor, void** argv)
	{
		// Allocate an instance of our class
		MonoObject* instance = mono_object_new(domain, classInstance);

		if (!classInstance)
		{
			LAB_CORE_ERROR("[ScriptEngine] The class could not be instantiated!");
			return nullptr;
		}

		mono_runtime_invoke(constructor, instance, argv, nullptr);

		return instance;
	}

}