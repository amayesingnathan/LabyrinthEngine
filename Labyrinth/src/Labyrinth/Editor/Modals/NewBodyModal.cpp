#include "Lpch.h"
#include "NewBodyModal.h"

#include <Labyrinth/ImGui/ImGuiWidgets.h>

namespace Laby {

	using BodyTypeEntry = ComboEntry<RigidBodyComponent::BodyType>;
	static constexpr BodyTypeEntry sBodyTypes[] =
	{
		{ "Static",				 RigidBodyComponent::BodyType::Static },
		{ "KinematicTexture2D",  RigidBodyComponent::BodyType::Kinematic },
		{ "DynamicSubTexture2D", RigidBodyComponent::BodyType::Dynamic }
	};

	using ShapeTypeEntry = ComboEntry<NewBodyModal::ShapeType>;
	static constexpr ShapeTypeEntry sShapeTypes[] =
	{
		{"Box",		NewBodyModal::ShapeType::Box},
		{"Circle",	NewBodyModal::ShapeType::Circle},
	};
}