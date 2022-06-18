#pragma once

#include <Labyrinth.h>

#include <imgui/imgui.h>
#include <glm/glm.hpp>

namespace Labyrinth {

	class BodySpecModal : public ModalWindow
	{
	public: // Class Types
		enum class Shape { Box, Circle };

		struct NoShapeOptions {};
		struct BoxOptions 
		{ 
			glm::vec2 halfExtents = { 0.5f, 0.5f };
			glm::vec2 offset = { 0.0f, 0.0f };
			float friction = 1.f;
			float density = 0.5f;
			float restitution = 0.f;
			float restitutionThreshold = 0.5f;
		};
		struct CircleOptions 
		{ 
			float radius = 0.5f;
			glm::vec2 offset = { 0.0f, 0.0f };
			float friction = 1.f;
			float density = 0.5f;
			float restitution = 0.f;
			float restitutionThreshold = 0.5f;
		};

		using Options = std::variant<NoShapeOptions, BoxOptions, CircleOptions>;

		struct BodyDef 
		{
			glm::vec2 position = { 0.0f, 0.0f };
			float rotation = 0.0f;
			glm::vec2 size = { 1.0f, 1.0f };
			RigidBodyComponent::BodyType type = RigidBodyComponent::BodyType::Static;
			bool fixedRotation = false;

			bool hasShape = false;
			Shape shape = Shape::Box;
			Options shapeDef = NoShapeOptions();
		};

	public:
		BodySpecModal(Ref<Scene>& data);

		void display() override;

	private:
		void ShapeOptions();
		void ConvertShapeOptions();

	private:
		Ref<Scene>& mContext;
		Entity mNewEntity;
		BodyDef mBodyDef;
	};

}
