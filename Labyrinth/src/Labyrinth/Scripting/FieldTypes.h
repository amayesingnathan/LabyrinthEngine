#pragma once

#include <Labyrinth/Core/System/Assert.h>
#include <Labyrinth/Scene/Entity.h>

#include <unordered_map>
#include <typeindex>

namespace Labyrinth {

	namespace Accessibility
	{
		enum : u8
		{
			None = 0,
			Private = (1 << 0),
			Internal = (1 << 1),
			Protected = (1 << 2),
			Public = (1 << 3),
		};
	};

	enum class ScriptFieldType
	{
		None = 0,
		Boolean,
		Int8, Int16, Int32, Int64,
		UInt8, UInt16, UInt32, UInt64,
		Float, Double,
		Vector2, Vector3, Vector4,
		Entity
	};

	class ScriptFieldTypes
	{
	public:
		static ScriptFieldType Get(const std::string& type) 
		{ 
			LAB_CORE_ASSERT(sFieldTypes.count(type) != 0);
			return sFieldTypes.at(type);
		}

		template<typename T>
		static bool IsValidType(ScriptFieldType type)
		{
			LAB_CORE_ASSERT(type != ScriptFieldType::None, "Not a valid field type!");

			return sSupportedTypes.at(type) == typeid(T);
		}

	private:
		inline static const std::unordered_map<std::string, ScriptFieldType> sFieldTypes =
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

		inline static const std::unordered_map<ScriptFieldType, std::type_index> sSupportedTypes =
		{
			{ ScriptFieldType::Boolean,	typeid(bool)},
			{ ScriptFieldType::Int8,	typeid(i8)},
			{ ScriptFieldType::Int16,	typeid(i16) },
			{ ScriptFieldType::Int32,	typeid(i32) },
			{ ScriptFieldType::Int64,	typeid(i64) },
			{ ScriptFieldType::UInt8,	typeid(u8) },
			{ ScriptFieldType::UInt16,	typeid(u16) },
			{ ScriptFieldType::UInt32,	typeid(u32) },
			{ ScriptFieldType::UInt64,	typeid(u64) },
			{ ScriptFieldType::Float,	typeid(f32) },
			{ ScriptFieldType::Double,	typeid(f64) },
			{ ScriptFieldType::Vector2,	typeid(glm::vec2)},
			{ ScriptFieldType::Vector3,	typeid(glm::vec3) },
			{ ScriptFieldType::Vector4,	typeid(glm::vec4) },
			{ ScriptFieldType::Entity,	typeid(UUID) }
		};
	};

	struct ScriptField
	{
		ScriptFieldType type;
		std::string name;
		MonoClassField* field;

		ScriptField(ScriptFieldType t, const std::string& n, MonoClassField* f) : type(t), name(n), field(f) {}
	};
}