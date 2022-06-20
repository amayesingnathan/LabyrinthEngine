#pragma once

#include <Labyrinth.h>

#include <imgui/imgui.h>
#include <glm/glm.hpp>

namespace Labyrinth {

	class BodySpecModal : public ModalWindow
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

		void display() override;

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
