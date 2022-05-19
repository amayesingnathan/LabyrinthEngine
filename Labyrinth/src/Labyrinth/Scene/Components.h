#pragma once

#include "Labyrinth/Maths/Vector2D.h"

#include "SceneCamera.h"
#include "Labyrinth/Renderer/SubTexture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Labyrinth {

	//////////////////////////////////////////
	//    General object data components	//
	//////////////////////////////////////////

	struct CameraComponent
	{
		SceneCamera camera;
		bool primary = true; //ToDo: Maybe move to Scene
		bool fixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
		CameraComponent(const SceneCamera & cam, bool prime = true, bool fixedAR = false)
			: camera(cam), primary(prime), fixedAspectRatio(fixedAR) {}
	

	};

	struct SpriteRendererComponent
	{
		enum class TexType { None = -1, Texture, Tile };

		union TextureComponent
		{
			void* noTex = nullptr;
			Ref<Texture2D> tex;
			Ref<SubTexture2D> subtex;

			TextureComponent() : noTex(nullptr) {}
			TextureComponent(Ref<Texture2D> t) : tex(t) {}
			TextureComponent(Ref<SubTexture2D> st) : subtex(st) {}
			TextureComponent(const TextureComponent& other)
			{
				memcpy(this, &other, sizeof(TextureComponent));
			}
			~TextureComponent() {}

			TextureComponent& operator= (const TextureComponent& other)
			{
				memcpy(this, &other, sizeof(TextureComponent));
				return *this;
			}
		};

		TexType type = TexType::None;

		uint8_t layer = 0;
		static const uint8_t MaxLayers = std::numeric_limits<uint8_t>::max();

		glm::vec4 colour{ 1.0f, 1.0f, 1.0f, 1.0f };
		TextureComponent texture;
		bool tile = false;
		float tilingFactor = 1.0f;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(uint8_t layer)
			: type(TexType::None), layer(layer), colour({ 1.0f, 1.0f, 1.0f, 1.0f }), tile(false) {}
		SpriteRendererComponent(const glm::vec4& rgba, uint8_t layer = 0)
			: type(TexType::None), layer(layer), colour(rgba), tile(false) {}
		SpriteRendererComponent(Ref<Texture2D> tex, float tf, uint8_t layer = 0)
			: type(TexType::Texture), layer(layer), texture(tex), tilingFactor(tf), tile(false) {}
		SpriteRendererComponent(Ref<SubTexture2D> subtex, float tf, uint8_t layer = 0)
			: type(TexType::Tile), layer(layer), texture(subtex), tilingFactor(tf), tile(true) {}

		bool hasTex() const { return type != TexType::None; }

		// Get normalised layer value
		float getNLayer() const { return (Cast<float>(layer) / Cast<float>(MaxLayers)); }
	};

	struct TagComponent
	{
		std::string tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string & tag) : tag(tag) {}

		operator std::string& () { return tag; }
		operator const std::string& () const { return tag; }

		operator const char* () { return tag.c_str(); }
	};

	struct TransformComponent
	{
		glm::vec3 translation = glm::vec3{ 0.0f };
		glm::vec3 rotation = glm::vec3{ 0.0f };
		glm::vec3 scale = glm::vec3{ 1.0f };

		glm::mat4 lastSafeTransform{ 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& trans, const glm::vec3& rot, const glm::vec3& sc) 
			: translation(trans), rotation(rot), scale(sc) {}

		glm::mat4 getTransform() const
		{
			glm::mat4 rot = glm::toMat4(glm::quat(rotation));

			return glm::translate(glm::mat4(1.0f), translation)
				* rot
				* glm::scale(glm::mat4(1.0f), scale);
		}

		// These operators function the same as "getTransform()" for easier 
		// implicit direct access when using the component
		operator glm::mat4 () 
		{ 
			glm::mat4 rot = glm::rotate(glm::mat4(1.0f), rotation.x, { 1, 0, 0 })
				* glm::rotate(glm::mat4(1.0f), rotation.y, { 0, 1, 0 })
				* glm::rotate(glm::mat4(1.0f), rotation.z, { 0, 0, 1 });

			return glm::translate(glm::mat4(1.0f), translation)
				* rot
				* glm::scale(glm::mat4(1.0f), scale);
		}

		operator const glm::mat4 () const 
		{
			glm::mat4 rot = glm::rotate(glm::mat4(1.0f), rotation.x, { 1, 0, 0 })
				* glm::rotate(glm::mat4(1.0f), rotation.y, { 0, 1, 0 })
				* glm::rotate(glm::mat4(1.0f), rotation.z, { 0, 0, 1 });

			return glm::translate(glm::mat4(1.0f), translation)
				* rot
				* glm::scale(glm::mat4(1.0f), scale);
		}
	};
}