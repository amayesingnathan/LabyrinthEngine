#pragma once

#include "Labyrinth/Core/System/Base.h"

#include <yaml-cpp/yaml.h>

#include <glm/glm.hpp>

namespace Labyrinth {

	class Scene;

	class LAB_API YAMLParser
	{
	public:
		enum class Type {Both = 0, Input, Output};

	public:
		YAMLParser() {}

		YAMLParser(const std::string& filepath)
		{
			 mIn = YAML::LoadFile(filepath);
		}

		const char* getData() const 
		{
			return mOut.c_str(); 
		}

		operator bool() { if (mIn) return true; else return false; }

		template<typename T>
		void EncodeObject(const T& data, bool flag = true);

		template<typename T>
		T DecodeObject();
		template<typename T>
		void DecodeObject(T& output);
		template<typename T, typename Target>
		Ref<T> DecodeObject(Target target);

	private:
		template<typename T, typename Target>
		Ref<T> DecodeObject(Target target, YAML::Node node);

	private: //Output API
		void BeginObject(const std::string& name)
		{
			mOut << YAML::Key << name;
			mOut << YAML::BeginMap; 
		}
		void BeginObject()
		{
			mOut << YAML::BeginMap;
		}

		void BeginSequence(const std::string& key)
		{
			mOut << YAML::Key << key << YAML::Value << YAML::BeginSeq;
		}
		void EndSequence()
		{
			mOut << YAML::EndSeq;
		}

		template<typename Value>
		void ObjectProperty(const std::string& key, const Value& value)
		{
			mOut << YAML::Key << key << YAML::Value << value;
		}

		void EndObject()
		{
			mOut << YAML::EndMap;
		}

	private:
		YAML::Node mIn;
		YAML::Emitter mOut;
	};
}