#include "BodySpecModal.h"

#include "../Panels/ScenePanel.h"

#include <glm/gtc/type_ptr.hpp>

namespace Labyrinth {

    BodySpecModal::BodySpecModal(Ref<Scene>& data) : Modal(), mContext(data) {}

    void BodySpecModal::onImGuiRender()
    {
        ImGui::Text("Please enter the specifications for the new rigid body:");
        ImGui::NewLine();

        ImGui::DragFloat2("Position", glm::value_ptr(mBodyDef.trans.translation), 0.01f);
        ImGui::DragFloat("Rotation", &mBodyDef.trans.rotation.z, 0.1f, 0.0f, 360.f);
        ImGui::DragFloat2("Size", glm::value_ptr(mBodyDef.trans.scale), 0.01f, 0.0f);

        std::string bodyType("Static");
        if (ImGui::BeginCombo("Body Type", bodyType.c_str()))
        {
            for (const auto& [label, type] : mBodyTypes)
            {
                bool isSelected = bodyType == label;

                if (ImGui::Selectable(label.c_str(), isSelected))
                {
                    bodyType = label;
                    mBodyDef.body.type = type;
                }

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        ImGui::Checkbox("Fixed Rotation", &mBodyDef.body.fixedRotation);

        if (ImGui::Checkbox("Has Shape?", &mBodyDef.hasShape))
        {
            if (mBodyDef.hasShape)
                mBodyDef.shapeDef = BoxColliderComponent();
            else
                mBodyDef.shapeDef = NoCollider();
        }
        ImGui::SameLine();
        ImGui::Checkbox("Render?", &mAddRender);

        if (mBodyDef.hasShape) 
        {
            if (ImGui::BeginCombo("Shape Type", mShapeString.c_str()))
            {
                for (const auto& [label, type] : mShapeTypes)
                {
                    bool isSelected = mShapeString == label;

                    if (ImGui::Selectable(label.c_str(), isSelected))
                    {
                        mShapeString = label;
                        mBodyDef.shape = type;
                        switch (mBodyDef.shape)
                        {
                        case Shape::Box:
                            mBodyDef.shapeDef = BoxColliderComponent();
                            break;

                        case Shape::Circle:
                            mBodyDef.shapeDef = CircleColliderComponent();
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
            auto& trans = mNewEntity.replaceComponent<TransformComponent>(mBodyDef.trans);
            auto& rbc = mNewEntity.addComponent<RigidBodyComponent>(mBodyDef.body);

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
            if constexpr (std::is_same_v<T, BoxColliderComponent>)
            {
                ImGui::DragFloat2("Half-Extents", glm::value_ptr(arg.halfExtents), 0.01f, 0.0f, 100.0f);
                ImGui::DragFloat2("Offset", glm::value_ptr(arg.offset), 0.01f, 0.0f, 100.0f);
                ImGui::DragFloat("Friction", &arg.friction, 0.01f, 0.0f, 100.0f);
                ImGui::DragFloat("Density", &arg.density, 0.01f, 0.0f, 100.0f);
                ImGui::DragFloat("Restitution", &arg.restitution, 0.01f, 0.0f, 100.0f);
                ImGui::DragFloat("Restitution Threshold", &arg.restitutionThreshold, 0.01f, 0.0f, 100.0f);
            }
            else if constexpr (std::is_same_v<T, CircleColliderComponent>)
            {
                ImGui::DragFloat("Radius", &arg.radius, 0.01f, 0.0f, 100.0f);
                ImGui::DragFloat2("Offset", glm::value_ptr(arg.offset), 0.01f, 0.0f, 100.0f);
                ImGui::DragFloat("Friction", &arg.friction, 0.01f, 0.0f, 100.0f);
                ImGui::DragFloat("Density", &arg.density, 0.01f, 0.0f, 100.0f);
                ImGui::DragFloat("Restitution", &arg.restitution, 0.01f, 0.0f, 100.0f);
                ImGui::DragFloat("Restitution Threshold", &arg.restitutionThreshold, 0.01f, 0.0f, 100.0f);
            }
            else if constexpr (std::is_same_v<T, NoCollider>) {}
            else
                static_assert(std::always_false_v<T>, "non-exhaustive visitor!");
        }, mBodyDef.shapeDef);
    }

    void BodySpecModal::ConvertShapeOptions()
    {
        std::visit([this](auto& arg) 
        {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, BoxColliderComponent>)
            {
                mNewEntity.addComponent<BoxColliderComponent>(arg);
                if (mAddRender) mNewEntity.addComponent<SpriteRendererComponent>();
            }
            else if constexpr (std::is_same_v<T, CircleColliderComponent>)
            {
                mNewEntity.addComponent<CircleColliderComponent>(arg);
                if (mAddRender) mNewEntity.addComponent<CircleRendererComponent>();
            }
            else if constexpr (std::is_same_v<T, NoCollider>) {}
            else 
                static_assert(std::always_false_v<T>, "non-exhaustive visitor!");
        }, mBodyDef.shapeDef);
    }
}
