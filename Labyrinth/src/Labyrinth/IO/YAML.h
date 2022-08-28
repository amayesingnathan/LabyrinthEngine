#pragma once

#include "Labyrinth/Core/System/Base.h"

#include <yaml-cpp/yaml.h>

#include <glm/glm.hpp>

namespace YAML {

	template<>
	struct convert<glm::vec2>
	{
		inline static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			//node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		inline static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<f32>();
			rhs.y = node[1].as<f32>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec3>
	{
		inline static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			//node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		inline static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<f32>();
			rhs.y = node[1].as<f32>();
			rhs.z = node[2].as<f32>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		inline static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			//node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		inline static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<f32>();
			rhs.y = node[1].as<f32>();
			rhs.z = node[2].as<f32>();
			rhs.w = node[3].as<f32>();
			return true;
		}
	};
}

namespace Labyrinth {

	inline YAML::Emitter& operator<<(YAML::Emitter& mOut, const glm::vec2& v)
	{
		mOut << YAML::Flow;
		mOut << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return mOut;
	}

	inline YAML::Emitter& operator<<(YAML::Emitter& mOut, const glm::vec3& v)
	{
		mOut << YAML::Flow;
		mOut << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return mOut;
	}

	inline YAML::Emitter& operator<<(YAML::Emitter& mOut, const glm::vec4& v)
	{
		mOut << YAML::Flow;
		mOut << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return mOut;
	}

}

#define LAB_SERIALISE_PROPERTY(propName, propVal, outputNode) outputNode << YAML::Key << #propName << YAML::Value << propVal

#define LAB_SERIALISE_PROPERTY_ASSET(propName, propVal, outputData) outputData << YAML::Key << #propName << YAML::Value << (propVal ? (u64)propVal->handle : 0);

#define LAB_DESERIALISE_PROPERTY_DEF(propName, destination, node, defaultValue) destination = node[#propName] ? node[#propName].as<decltype(defaultValue)>() : defaultValue
#define LAB_DESERIALISE_PROPERTY(propName, destination, node) destination = node[#propName] ? node[#propName].as<decltype(destination)>() : decltype(destination)()

#define LAB_DESERIALISE_PROPERTY_ASSET(propName, destination, inputData, assetClass)\
		{AssetHandle assetHandle = inputData[#propName] ? inputData[#propName].as<u64>() : 0;\
		if (AssetManager::IsAssetHandleValid(assetHandle))\
		{ destination = AssetManager::GetAsset<assetClass>(assetHandle); }\
		else\
		{ LAB_CORE_ERROR("Tried to load invalid asset {0}.", #assetClass); }}