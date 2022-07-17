#include "Lpch.h"
#include "Scripting.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/object.h>
#include <mono/metadata/attrdefs.h>

namespace Labyrinth {

	namespace Scripting {

		char* ReadBytes(const std::string& filepath, uint32_t* outSize)
		{
			std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

			if (!stream)
			{
				// Failed to open the file
				return nullptr;
			}

			std::streampos end = stream.tellg();
			stream.seekg(0, std::ios::beg);
			uint32_t size = end - stream.tellg();

			if (size == 0)
			{
				// File is empty
				LAB_CORE_ERROR("File was empty!");
				return nullptr;
			}

			char* buffer = new char[size];
			stream.read((char*)buffer, size);
			stream.close();

			*outSize = size;
			return buffer;
		}

		MonoAssembly* LoadCSharpAssembly(const std::string& assemblyPath)
		{
			uint32_t fileSize = 0;
			char* fileData = ReadBytes(assemblyPath, &fileSize);

			// NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
			MonoImageOpenStatus status;
			MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

			if (status != MONO_IMAGE_OK)
			{
				const char* errorMessage = mono_image_strerror(status);
				LAB_CORE_ERROR(errorMessage);
				return nullptr;
			}

			MonoAssembly* assembly = mono_assembly_load_from_full(image, assemblyPath.c_str(), &status, 0);
			mono_image_close(image);

			// Don't forget to free the file data
			delete[] fileData;

			return assembly;
		}

		void PrintAssemblyTypes(MonoAssembly* assembly)
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

		MonoClass* GetClassInAssembly(MonoAssembly* assembly, const char* namespaceName, const char* className)
		{
			MonoImage* image = mono_assembly_get_image(assembly);
			MonoClass* klass = mono_class_from_name(image, namespaceName, className);

			if (klass == nullptr)
			{
				LAB_CORE_ERROR("The class could not be loaded!");
				return nullptr;
			}

			return klass;
		}

		MonoObject* InstantiateClass(MonoDomain* domain, MonoAssembly* assembly, const char* namespaceName, const char* className)
		{
			MonoClass* klass = GetClassInAssembly(assembly, namespaceName, className);

			// Allocate an instance of our class
			MonoObject* classInstance = mono_object_new(domain, klass);

			if (classInstance == nullptr)
			{
				LAB_CORE_ERROR("The class could not be instantiated!");
				return nullptr;
			}

			mono_runtime_object_init(classInstance);

			return classInstance;
		}

		void CallMethod(MonoObject* instance, const char* methodName, void** argv, int argc)
		{
			MonoMethod* method = mono_class_get_method_from_name(mono_object_get_class(instance), methodName, argc);

			mono_runtime_invoke(method, instance, argv, nullptr);
		}

		uint8_t GetFieldAccessibility(MonoClassField* field)
		{
			uint8_t accessibility = Accessibility::None;
			uint32_t accessFlag = mono_field_get_flags(field) & MONO_FIELD_ATTR_FIELD_ACCESS_MASK;

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

		uint8_t GetPropertyAccessibility(MonoProperty* property)
		{
			uint8_t accessibility = Accessibility::None;

			// Get a reference to the property's getter method
			MonoMethod* propertyGetter = mono_property_get_get_method(property);
			if (propertyGetter != nullptr)
			{
				// Extract the access flags from the getters flags
				uint32_t accessFlag = mono_method_get_flags(propertyGetter, nullptr) & MONO_METHOD_ATTR_ACCESS_MASK;

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
			}

			// Get a reference to the property's setter method
			MonoMethod* propertySetter = mono_property_get_set_method(property);
			if (propertySetter != nullptr)
			{
				// Extract the access flags from the setters flags
				uint32_t accessFlag = mono_method_get_flags(propertySetter, nullptr) & MONO_METHOD_ATTR_ACCESS_MASK;
				if (accessFlag != MONO_FIELD_ATTR_PUBLIC)
					accessibility = Accessibility::Private;
			}
			else
			{
				accessibility = Accessibility::Private;
			}

			return accessibility;
		}

		bool CheckMonoError(MonoError& error)
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

	}
}