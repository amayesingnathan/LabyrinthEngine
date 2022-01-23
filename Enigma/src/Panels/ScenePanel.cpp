#include "ScenePanel.h"

#include <Labyrinth.h>

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>

#include "Labyrinth/Scene/Components.h"

namespace Labyrinth {

	extern const std::filesystem::path gAssetPath;

	ScenePanel::ScenePanel(const Ref<Scene>& scene)
	{
		setContext(scene);
	}

	void ScenePanel::setContext(const Ref<Scene>& scene)
	{
		mContext = scene;
		mSelectedEntity = {};
	}

	void ScenePanel::onImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		mContext->mRegistry.view<RootComponent>().each([&](auto entityID, auto& rc)
			{
				Entity entity{ entityID , mContext };
				DrawEntityNode(entity);
			});

		if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered())
			mSelectedEntity = {};

		if (ImGui::BeginPopupContextWindow(0, 1, false))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
				mContext->CreateEntity("Empty Entity");

			ImGui::EndPopup();
		}

		ImGui::End();

		ImGui::Begin("Properties");
		if (mSelectedEntity)
			DrawComponents();

		ImGui::End();
	}

	void ScenePanel::setSelectedEntity(Entity entity)
	{
		if (!entity) return;

		if (entity.getScene() == mContext)
		{
			mSelectedEntity = entity;
		}
		else LAB_WARN("Entity {0} is not part of the current scene!", entity.getUUID());
	}

	void ScenePanel::DrawEntityNode(Entity entity)
	{
		std::string& tag = entity.getComponent<TagComponent>();
		auto& node = entity.getComponent<NodeComponent>();

		ImGuiTreeNodeFlags flags = ((mSelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		if (ImGui::IsItemClicked())
		{
			mSelectedEntity = entity;
		}
		if (ImGui::BeginDragDropSource())
		{
			Entity* dragEntity = &entity;
			ImGui::SetDragDropPayload("ENTITY_ITEM", dragEntity, sizeof(Entity));
			ImGui::EndDragDropSource();
		}
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_ITEM"))
			{
				Entity* dragEntity = Cast<Entity>(payload->Data);
				if (*dragEntity != entity)
				{
					dragEntity->setParent(entity);
				}
			}
			ImGui::EndDragDropTarget();
		}

		bool childCreated = false;
		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Create Child"))
				childCreated = true;
			if (ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;

			ImGui::EndPopup();
		}

		if (opened)
		{
			for (auto& child : node.children)
				DrawEntityNode(child);
			ImGui::TreePop();
		}
		if (childCreated)
			mContext->CreateEntity("Empty Entity", entity);

		if (entityDeleted)
		{
			mContext->DestroyEntity(entity);
			if (mSelectedEntity == entity)
				mSelectedEntity = {};
		}
	}

	static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
			values.x = resetValue;
		ImGui::PopFont();	
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
			values.z = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}


	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
		if (entity.hasComponent<T>())
		{
			auto& component = entity.getComponent<T>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
			ImGui::PopStyleVar(
			);
			ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
			if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
			{
				ImGui::OpenPopup("ComponentSettings");
			}

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove component"))
					removeComponent = true;

				ImGui::EndPopup();
			}

			if (open)
			{
				uiFunction(component);
				ImGui::TreePop();
			}

			if (removeComponent)
				entity.removeComponent<T>();
		}
	}

	void ScenePanel::DrawComponents()
	{
		if (mSelectedEntity.hasComponent<TagComponent>())
		{
			std::string& tag = mSelectedEntity.getComponent<TagComponent>();

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			if (ImGui::MenuItem("Camera"))
			{
				if (!mSelectedEntity.hasComponent<CameraComponent>())
					mSelectedEntity.addComponent<CameraComponent>();
				else
					LAB_CORE_WARN("This entity already has the Camera Component!");
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Sprite Renderer"))
			{
				if (!mSelectedEntity.hasComponent<SpriteRendererComponent>())
					mSelectedEntity.addComponent<SpriteRendererComponent>();
				else
					LAB_CORE_WARN("This entity already has the Sprite Renderer Component!");
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();

		if (mSelectedEntity.hasComponent<NodeComponent>())
		{
			std::pair<std::string, Entity> noParent("None", Entity());
			std::unordered_map<std::string, Entity> possibleParents;
			possibleParents.emplace(noParent);

			// Create map of possible 
			mContext->mRegistry.group<TagComponent>(entt::get<IDComponent>).each([&](auto entityID, auto& tc, auto& idc) {
				if (mSelectedEntity != entityID)
				{
					possibleParents.emplace(tc.tag + "    (ID = " + idc.id.to_string() + ")", Entity{entityID, mContext});
				}
			});

			std::string currentParentString = "None";
			auto& parent = mSelectedEntity.getParent();
			if (parent)
				currentParentString = parent.getComponent<TagComponent>().tag + "    (ID =" + parent.getUUID().to_string() + ")";

			if (ImGui::BeginCombo("Parent", currentParentString.c_str()))
			{
				bool clear = currentParentString == noParent.first;
				if (ImGui::Selectable(noParent.first.c_str(), clear))
					if (mSelectedEntity.setParent(Entity()))
						currentParentString = noParent.first;

				for (auto [name, parentEnt] : possibleParents)
				{
					//Ignore "None" has deliberately done first so it is top of list.
					if (name == noParent.first)
						continue;

					bool isSelected = currentParentString == name;

					if (ImGui::Selectable(name.c_str(), isSelected))
						if (mSelectedEntity.setParent(parentEnt))
							currentParentString = name;

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}
		}

		DrawComponent<TransformComponent>("Transform", mSelectedEntity, [](auto& component)
		{
			DrawVec3Control("Translation", component.translation);
			glm::vec3 rotation = glm::degrees(component.rotation);
			DrawVec3Control("Rotation", rotation);
			component.rotation = glm::radians(rotation);
			DrawVec3Control("Scale", component.scale, 1.0f);
		});
		
		DrawComponent<CameraComponent>("Camera", mSelectedEntity, [this](auto& component)
		{
			auto& camera = component.camera;

			if (ImGui::Checkbox("Primary", &component.primary))
			{
				//Ensure only one camera can be primary at once
				if (component.primary)
				{
					auto cams = mContext->mRegistry.view<CameraComponent>();
					for (auto id : cams)
					{
						if (mSelectedEntity != id)
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
				float perspectiveVerticalFov = glm::degrees(camera.getPerspectiveVerticalFOV());
				if (ImGui::DragFloat("Vertical FOV", &perspectiveVerticalFov))
					camera.setPerspectiveVerticalFOV(glm::radians(perspectiveVerticalFov));

				float perspectiveNear = camera.getPerspectiveNearClip();
				if (ImGui::DragFloat("Near", &perspectiveNear))
					camera.setPerspectiveNearClip(perspectiveNear);

				float perspectiveFar = camera.getPerspectiveFarClip();
				if (ImGui::DragFloat("Far", &perspectiveFar))
					camera.setPerspectiveFarClip(perspectiveFar);
			}
			if (camera.getProjectionType() == SceneCamera::ProjectionType::Orthographic)
			{
				float orthoSize = camera.getOrthographicSize();
				if (ImGui::DragFloat("Size", &orthoSize))
					camera.setOrthographicSize(orthoSize);

				float orthoFar = camera.getOrthographicFarClip();
				if (ImGui::DragFloat("Far", &orthoFar))
					camera.setOrthographicFarClip(orthoFar);

				ImGui::Checkbox("Fixed Aspect Ratio", &component.fixedAspectRatio);
			}
		});

		DrawComponent<SpriteRendererComponent>("Sprite Renderer", mSelectedEntity, [](auto& component)
		{
			ImGui::ColorEdit4("Colour", glm::value_ptr(component.colour));

			ImGui::Button("Texture", ImVec2(100.0f, 0.0f));
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					std::filesystem::path texturePath = std::filesystem::path(gAssetPath) / path;
					component.texture = Texture2D::Create(texturePath.string());
					component.type = SpriteRendererComponent::TexType::Texture;
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::DragFloat("Tiling Factor", &component.tilingFactor, 0.1f, 0.0f, 100.0f);
		});

	}

}