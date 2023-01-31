#include "Lpch.h"
#include "NewBodyModal.h"

#include <Labyrinth/ImGui/ImGuiWidgets.h>
#include <Labyrinth/Tools/EnumUtils.h>

namespace Laby {

	using BodyTypeEntry = ComboEntry<RigidBodyComponent::BodyType>;
	static constexpr std::array<BodyTypeEntry, 3> sBodyTypes =
	{
		BodyTypeEntry{ "Static",			  RigidBodyComponent::BodyType::Static },
		BodyTypeEntry{ "KinematicTexture2D",  RigidBodyComponent::BodyType::Kinematic },
		BodyTypeEntry{ "DynamicSubTexture2D", RigidBodyComponent::BodyType::Dynamic }
	};

	using ShapeTypeEntry = ComboEntry<NewBodyModal::ShapeType>;
	static constexpr std::array<ShapeTypeEntry, 2> sShapeTypes =
	{
		ShapeTypeEntry{"Box",		NewBodyModal::ShapeType::Box},
		ShapeTypeEntry{"Circle",	NewBodyModal::ShapeType::Circle},
	};

	NewBodyModal::NewBodyModal(const Ref<Scene>& scene)
		: mContext(scene)
	{
	}

	void NewBodyModal::onImGuiRender()
	{
		Widgets::Label("Please enter the specifications for the new rigid body:");
		Widgets::NewLine();

		Widgets::Vector2Edit("Position", *(glm::vec2*)(&mNewBodyDef.trans.translation));
		Widgets::FloatEdit("Rotation", glm::degrees(mNewBodyDef.trans.rotation.z), [&](f32 r) { mNewBodyDef.trans.rotation.z = glm::radians(r); });
		Widgets::Vector2Edit("Size", *(glm::vec2*)(&mNewBodyDef.trans.scale));

		Widgets::Combobox<RigidBodyComponent::BodyType>("Body Type", Enum::ToString(mNewBodyDef.body.type), mNewBodyDef.body.type, sBodyTypes);
		Widgets::Checkbox("Fixed Rotation", mNewBodyDef.body.fixedRotation);

		Widgets::Checkbox("Has Shape?", mNewBodyDef.hasShape, [&]()
		{
			if (mNewBodyDef.hasShape)
                mNewBodyDef.shapeDef = BoxColliderComponent();
            else
                mNewBodyDef.shapeDef = NoCollider();
		});
		Widgets::SameLine();
		Widgets::Checkbox("Render?", mNewBodyDef.addRender);

        if (!mNewBodyDef.hasShape)
            return;

        Widgets::Combobox<NewBodyModal::ShapeType>("Shape Type", Enum::ToString(mNewBodyDef.shape), mNewBodyDef.shape, sShapeTypes,
        [&](std::string_view key, auto value)
        {
            mNewBodyDef.shape = value;
            switch (mNewBodyDef.shape)
            {
            case ShapeType::Box:
                mNewBodyDef.shapeDef = BoxColliderComponent(); break;
            case ShapeType::Circle:
                mNewBodyDef.shapeDef = CircleColliderComponent(); break;
            }
        });
	}

	void NewBodyModal::onComplete()
	{
        mNewEntity = mContext->CreateEntity("Rigid Body");
        auto& trans = mNewEntity.replaceComponent<TransformComponent>(mNewBodyDef.trans);
        auto& rbc = mNewEntity.addComponent<RigidBodyComponent>(mNewBodyDef.body);

        if (mNewBodyDef.hasShape)
            ConvertShapeOptions();
	}

	void NewBodyModal::ShapeOptions()
	{
        std::visit([](auto& arg)
        {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, BoxColliderComponent>)
            {
                Widgets::Vector2Edit("Half-Extents", arg.halfExtents);
                Widgets::Vector2Edit("Offset", arg.offset);
                Widgets::FloatEdit("Friction", arg.friction, 0.01f, 0.0f, 100.0f);
                Widgets::FloatEdit("Density", arg.density, 0.01f, 0.0f, 100.0f);
                Widgets::FloatEdit("Restitution", arg.restitution, 0.01f, 0.0f, 100.0f);
                Widgets::FloatEdit("Restitution Threshold", arg.restitutionThreshold, 0.01f, 0.0f, 100.0f);
				Widgets::Checkbox("Sensor", arg.sensor);
            }
            else if constexpr (std::is_same_v<T, CircleColliderComponent>)
            {
                Widgets::FloatEdit("Radius", arg.radius, 0.01f, 0.0f, 100.0f);
                Widgets::Vector2Edit("Offset", arg.offset);
                Widgets::FloatEdit("Friction", arg.friction, 0.01f, 0.0f, 100.0f);
                Widgets::FloatEdit("Density", arg.density, 0.01f, 0.0f, 100.0f);
                Widgets::FloatEdit("Restitution", arg.restitution, 0.01f, 0.0f, 100.0f);
                Widgets::FloatEdit("Restitution Threshold", arg.restitutionThreshold, 0.01f, 0.0f, 100.0f);
				Widgets::Checkbox("Sensor", arg.sensor);
            }
            else if constexpr (std::is_same_v<T, NoCollider>) 
                ;
            else
                LAB_STATIC_ASSERT(std::always_false_v<T>, "non-exhaustive visitor!");
        }, mNewBodyDef.shapeDef);
	}

	void NewBodyModal::ConvertShapeOptions()
	{
        std::visit([this](auto& arg) 
        {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, BoxColliderComponent>)
            {
                mNewEntity.addComponent<BoxColliderComponent>(arg);
                if (mNewBodyDef.addRender) mNewEntity.addComponent<SpriteRendererComponent>();
            }
            else if constexpr (std::is_same_v<T, CircleColliderComponent>)
            {
                mNewEntity.addComponent<CircleColliderComponent>(arg);
                if (mNewBodyDef.addRender) mNewEntity.addComponent<CircleRendererComponent>();
            }
            else if constexpr (std::is_same_v<T, NoCollider>)
                ;
            else 
                LAB_STATIC_ASSERT(std::always_false_v<T>, "non-exhaustive visitor!");
        }, mNewBodyDef.shapeDef);
	}
}