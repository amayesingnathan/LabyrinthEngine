#pragma once

#include "Modal.h"

#include <Labyrinth/Scene/Scene.h>
#include <Labyrinth/Scene/Entity.h>
#include <Labyrinth/Scene/Components.h>

#include <imgui/imgui.h>
#include <glm/glm.hpp>

namespace Labyrinth {

	class BodySpecModal : public Modal
	{
	public: // Class Types
		enum class Shape { Box, Circle };

		struct NoCollider {};
		using Collider = std::variant<NoCollider, BoxColliderComponent, CircleColliderComponent>;

		struct BodyDef 
		{
			TransformComponent trans;
			RigidBodyComponent body;

			bool hasShape = false;
			Shape shape = Shape::Box;
			Collider shapeDef = NoCollider();
		};

	public:
		BodySpecModal(Ref<Scene>& data);

		void onImGuiRender() override;

		static Ref<BodySpecModal> Create(Ref<Scene>& data) { return CreateRef<BodySpecModal>(data); }

	private:
		void ShapeOptions();
		void ConvertShapeOptions();

	private:
		Ref<Scene>& mContext;
		Entity mNewEntity;
		BodyDef mBodyDef;
		std::string mShapeString = "Box";
		bool mAddRender = false;

		struct BodyType { std::string label;  RigidBodyComponent::BodyType type; };
		const std::vector<BodyType> mBodyTypes =
		{
			{"Static", RigidBodyComponent::BodyType::Static },
			{"Kinematic", RigidBodyComponent::BodyType::Kinematic },
			{"Dynamic", RigidBodyComponent::BodyType::Dynamic }
		};

		struct Shapes { std::string label;  Shape shape; };
		const std::vector<Shapes> mShapeTypes =
		{
			{"Box", Shape::Box},
			{"Circle", Shape::Circle},
		};
	};

}
