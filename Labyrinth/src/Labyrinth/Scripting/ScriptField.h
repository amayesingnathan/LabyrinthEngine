#pragma once

#include "ScriptDefinitions.h"

#include "Labyrinth/Containers/Buffer.h"
#include <Labyrinth/Scene/Entity.h>

#include <typeindex>

namespace Laby {

	namespace Accessibility
	{
		enum : u8
		{
			None = 0,
			Private = LAB_BIT(0),
			Internal = LAB_BIT(1),
			Protected = LAB_BIT(2),
			Public = LAB_BIT(3),
		};
	};

	enum class ScriptFieldType
	{
		None = -1,
		Boolean,
		Int8, Int16, Int32, Int64,
		UInt8, UInt16, UInt32, UInt64,
		Float, Double,
		Vector2, Vector3, Vector4,
		Entity,
	};

	namespace ScriptFields {

		static ScriptFieldType GetType(std::string_view type)
		{
			static const std::unordered_map<std::string_view, ScriptFieldType> sFieldTypes =
			{
				{ "System.Boolean",		ScriptFieldType::Boolean},
				{ "System.SByte",		ScriptFieldType::Int8},
				{ "System.Int16",		ScriptFieldType::Int16 },
				{ "System.Int32",		ScriptFieldType::Int32 },
				{ "System.Int64",		ScriptFieldType::Int64 },
				{ "System.SByte",		ScriptFieldType::UInt8 },
				{ "System.UInt16",		ScriptFieldType::UInt16 },
				{ "System.UInt32",		ScriptFieldType::UInt32 },
				{ "System.UInt64",		ScriptFieldType::UInt64 },
				{ "System.Single",		ScriptFieldType::Float },
				{ "System.Double",		ScriptFieldType::Double },
				{ "Labyrinth.Vector2",	ScriptFieldType::Vector2},
				{ "Labyrinth.Vector3",	ScriptFieldType::Vector3 },
				{ "Labyrinth.Vector4",	ScriptFieldType::Vector4 },
				{ "Labyrinth.Entity",	ScriptFieldType::Entity }
			};

			LAB_CORE_ASSERT(sFieldTypes.contains(type));
			return sFieldTypes.at(type);
		}
	}

	using FieldTypes = TypeList
	<
		bool, 
		i8, i16, i32, i64,
		u8, u16, u32, u64,
		f32, f64,
		glm::vec2, glm::vec3, glm::vec4,
		UUID
	>;

	template<typename T>
	concept IsFieldType = FieldTypes::Contains<T>;

	struct ScriptField
	{
		ScriptFieldType type = ScriptFieldType::None;
		std::string_view name;
		MonoClassField* field = nullptr;

		ScriptField() = default;
		ScriptField(ScriptFieldType t, std::string_view n, MonoClassField* f) : type(t), name(n), field(f) {}
	};


	struct FieldBuffer : public Buffer
	{
		FieldBuffer() = default;
		FieldBuffer(ScriptFieldType type);

		template<IsFieldType T>
		void set(const T& val = T())
		{
			constexpr usize newSize = sizeof(T);
			if (size() != newSize)
			{
				release();
				allocate(newSize);
			}

			memcpy(mData, &val, newSize);
		}

		template<IsFieldType T>
		T& get() { return read<T>(); }
		template<IsFieldType T>
		const T& get() const { return read<T>(); }

		void clear() { release(); }
	};

	struct FieldInitialiser
	{
		ScriptFieldType type = ScriptFieldType::None;
		FieldBuffer value;

		FieldInitialiser() = default;
		FieldInitialiser(const ScriptField& f);
	};

	using FieldValues = std::unordered_map<std::string, FieldInitialiser>;
}