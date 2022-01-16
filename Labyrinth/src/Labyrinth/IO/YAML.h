#pragma once

#include "Labyrinth/Core/Base.h"

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


		template<typename T> 
		void serialiseObject(T& data);

		template<typename T>
		void serialiseComponent(class Entity& entity)
		{
			T& component = entity.getComponent<T>();
			serialiseComponent<T>(component);
		}
		template<typename T>
		void serialiseComponent(T& data);


		bool deserialiseScene(Ref<Scene> scene);
		bool deserialiseEntity(Ref<Scene> scene);
		bool deserialiseEntity(Ref<Scene> scene, YAML::Node entity);

		template<typename T>
		T* deserialiseComponent(class Entity& entity, YAML::Node node);

		operator bool() { if (mIn) return true; else return false; }

	private:
		YAML::Node mIn;
		YAML::Emitter mOut;
	};
}