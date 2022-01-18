#pragma once

#include "Labyrinth/Core/System/Base.h"

#include <yaml-cpp/yaml.h>

#include <glm/glm.hpp>

namespace Labyrinth {

	class Scene;

	class YAMLParser
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
		void encodeObject(const T& data);

		template<typename T>
		T decodeObject();
		template<typename T, typename Target>
		Ref<T> decodeObject(Target target);

	private:
		template<typename T, typename Target>
		Ref<T> decodeObject(Target target, YAML::Node node);



		//template<typename T> 
		//void serialiseObject(T& data);

		//template<typename T>
		//void serialiseComponent(class Entity& entity)
		//{
		//	T& component = entity.getComponent<T>();
		//	serialiseComponent<T>(component);
		//}
		//template<typename T>
		//void serialiseComponent(T& data);


		//bool deserialiseScene(Ref<Scene> scene);
		//bool deserialiseEntity(Ref<Scene> scene);
		//bool deserialiseEntity(Ref<Scene> scene, YAML::Node entity);

		//template<typename T>
		//T* deserialiseComponent(class Entity& entity, YAML::Node node);


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
		void ObjectProperty(const std::string& key, Value value)
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