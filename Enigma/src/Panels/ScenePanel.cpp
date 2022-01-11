#include "ScenePanel.h"

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>

#include "Labyrinth/Scene/Components.h"

namespace Labyrinth {

	ScenePanel::ScenePanel(const Ref<Scene>& scene)
	{
		setContext(scene);
	}

	void ScenePanel::setContext(const Ref<Scene>& scene)
	{
		mContext = scene;
	}

	void ScenePanel::onImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		mContext->mRegistry.each([&](auto entityID)
			{
				Entity entity{ entityID , mContext.get() };
				DrawEntityNode(entity);
			});

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			mSelectionContext = {};

		ImGui::End();

		ImGui::Begin("Properties");
		if (mSelectionContext)
			DrawComponents(mSelectionContext);

		ImGui::End();
	}

	void ScenePanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.getComponent<TagComponent>().tag;

		ImGuiTreeNodeFlags flags = ((mSelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		if (ImGui::IsItemClicked())
		{
			mSelectionContext = entity;
		}

		if (opened)
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
			bool opened = ImGui::TreeNodeEx((void*)9817239, flags, tag.c_str());
			if (opened)
				ImGui::TreePop();
			ImGui::TreePop();
		}
	}

	void ScenePanel::DrawComponents(Entity entity)
	{
		if (entity.hasComponent<TagComponent>())
		{
			std::string& tag = entity.getComponent<TagComponent>();

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());
			if (ImGui::InputText("Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		if (entity.hasComponent<TransformComponent>())
		{
			if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
			{
				glm::mat4& transform = entity.getComponent<TransformComponent>();
				ImGui::DragFloat3("Position", glm::value_ptr(transform[3]), 0.1f);

				ImGui::TreePop();
			}
		}

		if (entity.hasComponent<SpriteRendererComponent>())
		{
			if (ImGui::TreeNodeEx((void*)typeid(SpriteRendererComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Colour"))
			{
				glm::vec4& colour = entity.getComponent<SpriteRendererComponent>().colour;
				ImGui::ColorEdit4("Colour", glm::value_ptr(colour));

				ImGui::TreePop();
			}
		}

		if (entity.hasComponent<CameraComponent>())
		{
			if (ImGui::TreeNodeEx((void*)typeid(CameraComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Camera"))
			{
				auto& cameraComponent = entity.getComponent<CameraComponent>();
				auto& camera = cameraComponent.camera;

				if (ImGui::Checkbox("Primary", &cameraComponent.primary))
				{
					if (cameraComponent.primary)
					{
						auto cams = mContext->mRegistry.view<CameraComponent>();
						for (auto id : cams)
						{
							if (entity != id)
							{
								auto& primaryOther = cams.get<CameraComponent>(id).primary;
								primaryOther = false;
							}
						}
					}
				}

				const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
				const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.getProjectionType()];
				if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
				{
					for (int i = 0; i < 2; i++)
					{
						bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
						if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
						{
							currentProjectionTypeString = projectionTypeStrings[i];
							camera.setProjectionType((SceneCamera::ProjectionType)i);
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}

				if (camera.getProjectionType() == SceneCamera::ProjectionType::Perspective)
				{
					float verticalFov = glm::degrees(camera.getPerspectiveVerticalFOV());
					if (ImGui::DragFloat("Vertical FOV", &verticalFov))
						camera.setPerspectiveVerticalFOV(glm::radians(verticalFov));

					float orthoNear = camera.getPerspectiveNearClip();
					if (ImGui::DragFloat("Near", &orthoNear))
						camera.setPerspectiveNearClip(orthoNear);

					float orthoFar = camera.getPerspectiveFarClip();
					if (ImGui::DragFloat("Far", &orthoFar))
						camera.setPerspectiveFarClip(orthoFar);
				}

				if (camera.getProjectionType() == SceneCamera::ProjectionType::Orthographic)
				{
					float orthoSize = camera.getOrthographicSize();
					if (ImGui::DragFloat("Size", &orthoSize))
						camera.setOrthographicSize(orthoSize);

					float orthoNear = camera.getOrthographicNearClip();
					if (ImGui::DragFloat("Near", &orthoNear))
						camera.setOrthographicNearClip(orthoNear);

					float orthoFar = camera.getOrthographicFarClip();
					if (ImGui::DragFloat("Far", &orthoFar))
						camera.setOrthographicFarClip(orthoFar);

					ImGui::Checkbox("Fixed Aspect Ratio", &cameraComponent.fixedAspectRatio);
				}


				ImGui::TreePop();
			}
		}
	}

}