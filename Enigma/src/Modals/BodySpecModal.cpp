#include "BodySpecModal.h"

#include "../Panels/ScenePanel.h"

#include <glm/gtc/type_ptr.hpp>

namespace Labyrinth {

    BodySpecModal::BodySpecModal(Ref<Scene>& data) : mContext(data) {}

    void BodySpecModal::display()
    {
        ImGui::Text("Please enter the specifications for the new rigid body:");
        ImGui::NewLine();

        ImGui::DragFloat2("Position", glm::value_ptr(mBodyDef.position), 0.01f, 0.0f, 100.0f);
        ImGui::DragFloat("Rotation", &mBodyDef.rotation, 0.01f, 0.0f, 100.0f);
        ImGui::DragFloat2("Size", glm::value_ptr(mBodyDef.size), 0.01f, 0.0f, 100.0f);

        struct BodyType { std::string label;  RigidBodyComponent::BodyType type; };
        const std::vector<BodyType> bodyTypes =
        {
            {"Static", RigidBodyComponent::BodyType::Static },
            {"Kinematic", RigidBodyComponent::BodyType::Kinematic },
            {"Dynamic", RigidBodyComponent::BodyType::Dynamic } 
        };

        std::string bodyType("Static");
        if (ImGui::BeginCombo("Body Type", bodyType.c_str()))
        {
            for (const auto& [label, type] : bodyTypes)
            {
                bool isSelected = bodyType == label;

                if (ImGui::Selectable(label.c_str(), isSelected))
                {
                    bodyType = label;
                    mBodyDef.type = type;
                }

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        ImGui::Checkbox("Fixed Rotation", &mBodyDef.fixedRotation);

        if (ImGui::Checkbox("Has Shape?", &mBodyDef.hasShape))
        {
            if (mBodyDef.hasShape)
                mBodyDef.shapeDef = BoxOptions();
            else
                mBodyDef.shapeDef = NoShapeOptions();
        }

        if (mBodyDef.hasShape) 
        {
            struct Shapes { std::string label;  Shape shape; };
            const std::vector<Shapes> shapeTypes =
            {
                {"Box", Shape::Box},
                {"Circle", Shape::Circle},
            };

            std::string shape("Box");
            if (ImGui::BeginCombo("Shape Type", shape.c_str()))
            {
                for (const auto& [label, type] : shapeTypes)
                {
                    bool isSelected = shape == label;

                    if (ImGui::Selectable(label.c_str(), isSelected))
                    {
                        shape = label;
                        mBodyDef.shape = type;
                        switch (mBodyDef.shape)
                        {
                        case Shape::Box:
                            mBodyDef.shapeDef = BoxOptions();
                            break;

                        case Shape::Circle:
                            mBodyDef.shapeDef = CircleOptions();
                            break;
                        }
                    }

                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }

                ImGui::EndCombo();
            }

            ShapeOptions();
        }

        if (ImGui::Button("OK"))
        {
            mNewEntity = mContext->CreateEntity("Rigid Body");
            mNewEntity.addComponent<SpriteRendererComponent>();
            auto& trans = mNewEntity.getComponent<TransformComponent>();
            trans.translation.x = mBodyDef.position.x;
            trans.translation.y = mBodyDef.position.y;
            trans.rotation.z = mBodyDef.rotation;
            trans.scale.x = mBodyDef.size.x;
            trans.scale.y = mBodyDef.size.y;

            auto& rbc = mNewEntity.addComponent<RigidBodyComponent>();
            rbc.type = mBodyDef.type;
            rbc.fixedRotation = mBodyDef.fixedRotation;

            if (mBodyDef.hasShape)
                ConvertShapeOptions();

            Close();
        }

        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
            Close();
    }

    void BodySpecModal::ShapeOptions()
    {
        std::visit([](auto& arg)
        {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, CircleOptions>)
            {
                ImGui::DragFloat("Radius", &arg.radius, 0.01f, 0.0f, 100.0f);
                ImGui::DragFloat2("Offset", glm::value_ptr(arg.offset), 0.01f, 0.0f, 100.0f);
                ImGui::DragFloat("Friction", &arg.friction, 0.01f, 0.0f, 100.0f);
                ImGui::DragFloat("Density", &arg.density, 0.01f, 0.0f, 100.0f);
                ImGui::DragFloat("Restitution", &arg.restitution, 0.01f, 0.0f, 100.0f);
                ImGui::DragFloat("Restitution Threshold", &arg.restitutionThreshold, 0.01f, 0.0f, 100.0f);
            }
            else if constexpr (std::is_same_v<T, BoxOptions>)
            {
                ImGui::DragFloat2("Half-Extents", glm::value_ptr(arg.halfExtents), 0.01f, 0.0f, 100.0f);
                ImGui::DragFloat2("Offset", glm::value_ptr(arg.offset), 0.01f, 0.0f, 100.0f);
                ImGui::DragFloat("Friction", &arg.friction, 0.01f, 0.0f, 100.0f);
                ImGui::DragFloat("Density", &arg.density, 0.01f, 0.0f, 100.0f);
                ImGui::DragFloat("Restitution", &arg.restitution, 0.01f, 0.0f, 100.0f);
                ImGui::DragFloat("Restitution Threshold", &arg.restitutionThreshold, 0.01f, 0.0f, 100.0f);
            }
            else if constexpr (std::is_same_v<T, NoShapeOptions>) {}
            else
                static_assert(std::always_false_v<T>, "non-exhaustive visitor!");
        }, mBodyDef.shapeDef);
    }

    void BodySpecModal::ConvertShapeOptions()
    {
        std::visit([this](auto& arg) 
        {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, BoxOptions>) {
                auto& bcc = mNewEntity.addComponent<BoxColliderComponent>();
                bcc.halfExtents = arg.halfExtents;
                bcc.offset = arg.offset;
                bcc.friction = arg.friction;
                bcc.density = arg.density;
                bcc.restitution = arg.restitution;
                bcc.restitutionThreshold = arg.restitutionThreshold;
            }
            else if constexpr (std::is_same_v<T, CircleOptions>) {
                auto& ccc = mNewEntity.addComponent<CircleColliderComponent>();
                ccc.radius = arg.radius;
                ccc.offset = arg.offset;
                ccc.friction = arg.friction;
                ccc.density = arg.density;
                ccc.restitution = arg.restitution;
                ccc.restitutionThreshold = arg.restitutionThreshold;
            }
            else if constexpr (std::is_same_v<T, NoShapeOptions>) {}
            else 
                static_assert(std::always_false_v<T>, "non-exhaustive visitor!");
        }, mBodyDef.shapeDef);
    }
}
