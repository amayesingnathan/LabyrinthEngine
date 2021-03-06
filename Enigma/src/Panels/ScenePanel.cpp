#include "ScenePanel.h"

#include "SpriteSheetData.h"
#include "../EditorLayer.h"
#include "../Modals/ModalManager.h"
#include "../Modals/BodySpecModal.h"
#include "../Modals/MapSpecModal.h"

#include <Labyrinth/Assets/AssetManager.h>
#include <Labyrinth/Renderer/Renderer2D.h>
#include <Labyrinth/Renderer/RenderCommand.h>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>

namespace Labyrinth {

	extern const std::filesystem::path gAssetPath;

	ScenePanel::ScenePanel()
	{
		mNoTex = AssetManager::GetOrCreate<Texture2D>("NoTex", "assets/textures/checkerboard.png");
	}

	ScenePanel::ScenePanel(EditorData& options)
	{
		mEditorData = &options;
		mNoTex = AssetManager::GetOrCreate<Texture2D>("NoTex", "assets/textures/checkerboard.png");
	}

	ScenePanel::ScenePanel(const Ref<Scene>& scene, EditorData& options)
	{
		mContext = scene;
		mEditorData = &options;

		mNoTex = AssetManager::GetOrCreate<Texture2D>("NoTex", "assets/textures/checkerboard.png");
	}

	void ScenePanel::setContext(const Ref<Scene>& scene)
	{
		mContext = scene;
		mSelectedEntity = {};
	}

	void ScenePanel::setContext(const Ref<Scene>& scene, EditorData& options)
	{
		mContext = scene;
		mEditorData = &options;
		mSelectedEntity = {};
	}

	void ScenePanel::onUpdate()
	{
		if (!mSelectedEntity || !mSelectedEntity.hasComponent<SpriteRendererComponent>()) return;

		auto& component = mSelectedEntity.getComponent<SpriteRendererComponent>();

		if (!mTexture)
		{
			FramebufferSpec fbSpec;
			fbSpec.width = 200;
			fbSpec.height = 200;
			fbSpec.attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
			fbSpec.samples = 1;

			mTexture = Framebuffer::Create(fbSpec);
		}

		// Draw texture/subtexture to framebuffer
		mTexture->bind();

		RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 0.0f });
		RenderCommand::Clear();

		Renderer2D::BeginState();

		std::string texLabel;
		switch (component.type)
		{
		case SpriteRendererComponent::TexType::None:
			Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 1.0f, 1.0f }, mNoTex);
			break;

		case SpriteRendererComponent::TexType::Texture:
			Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 1.0f, 1.0f }, component.getTex<Texture2D>());
			break;

		case SpriteRendererComponent::TexType::SubTexture:
			Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 1.0f, 1.0f }, component.getTex<SubTexture2D>());
			break;
		}

		Renderer2D::EndState();
		mTexture->unbind();
	}

	void ScenePanel::onImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		STR_COPY(buffer, mContext->getName());
		if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			mContext->setName(buffer);

		ImGui::SameLine();

		if (ImGui::Button("Create.."))
			ImGui::OpenPopup("CreateEntityPopup");

		bool openSpecModal = false;
		if (ImGui::BeginPopup("CreateEntityPopup"))
		{
			if (ImGui::MenuItem("Empty Entity"))
				mSelectedEntity = mContext->CreateEntity("Empty Entity");
			if (ImGui::MenuItem("Camera"))
			{
				mSelectedEntity = mContext->CreateEntity("Camera");
				mSelectedEntity.addComponent<CameraComponent>();
			}
			if (ImGui::MenuItem("Rigid Body"))
				ModalManager::Open<BodySpecModal>("BodySpecModal", mContext);
			ImGui::EndPopup();
		}

		mContext->mRegistry.view<RootComponent>().each([&](auto entityID, auto& rc)
		{
			Entity entity{ entityID , mContext };
			DrawEntityNode(entity);
		});

		for (auto& entity : mToRemove)
			mContext->DestroyEntity(entity, mEditorData ? mEditorData->linkOnDestroy : false);
		mToRemove.clear();

		if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered())
			mSelectedEntity = {};

		ImGui::End();

		ImGui::Begin("Properties");
		if (mSelectedEntity)
			DrawComponents();

		AssetTypeWarning();

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

		bool entityCreated = false;
		bool bodyCreated = false;
		bool childCreated = false;
		bool cloneEntity = false;
		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Create Entity"))
				entityCreated = true;
			if (ImGui::MenuItem("Create Rigid Body"))
				bodyCreated = true;
			if (ImGui::MenuItem("Create Child"))
				childCreated = true;
			if (ImGui::MenuItem("Clone Entity"))
				cloneEntity = true;
			if (ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;

			ImGui::EndPopup();
		}

		if (opened)
		{
			// Range for loop iterators may become invalidated if a new entity is added
			// to this node's children during looping, so ignore new additions for now
			// and they'll be drawn next render.
			size_t fixedSize = node.children.size();
			for (size_t i = 0; i < fixedSize; i++)
				DrawEntityNode({ node.children[i], mContext });
			ImGui::TreePop();
		}
		if (entityCreated)
			mContext->CreateEntity("Empty Entity");
		if (bodyCreated)
			ModalManager::Open<BodySpecModal>("BodySpecModal", mContext);
		if (childCreated)
			mContext->CreateEntity("Empty Entity", entity);
		if (cloneEntity)
			mContext->CloneEntity(entity);

		if (entityDeleted)
		{
			mToRemove.emplace_back(entity); // Queue up to delete after range for loop so it isn't invalidated.
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
			{
				entity.removeComponent<T>();
				removeComponent = false;
			}
		}
	}

	void ScenePanel::DrawComponents()
	{
		if (mSelectedEntity.hasComponent<TagComponent>())
		{
			std::string& tag = mSelectedEntity.getComponent<TagComponent>();

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			STR_COPY(buffer, tag);
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		bool addTileMap = false;
		if (ImGui::BeginPopup("AddComponent"))
		{
			if (!mSelectedEntity.hasComponent<CameraComponent>())
			{
				if (ImGui::MenuItem("Camera"))
				{
					auto& cam = mSelectedEntity.addComponent<CameraComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!mSelectedEntity.hasComponent<SpriteRendererComponent>())
			{
				if (ImGui::MenuItem("Sprite Renderer"))
				{
					mSelectedEntity.addComponent<SpriteRendererComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!mSelectedEntity.hasComponent<CircleRendererComponent>())
			{
				if (ImGui::MenuItem("Circle Renderer"))
				{
					mSelectedEntity.addComponent<CircleRendererComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!mSelectedEntity.hasComponent<RigidBodyComponent>())
			{
				if (ImGui::MenuItem("Rigid Body"))
				{
					mSelectedEntity.addComponent<RigidBodyComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!mSelectedEntity.hasComponent<BoxColliderComponent>())
			{
				if (ImGui::MenuItem("Box Collider"))
				{
					mSelectedEntity.addComponent<BoxColliderComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!mSelectedEntity.hasComponent<CircleColliderComponent>())
			{
				if (ImGui::MenuItem("Circle Collider"))
				{
					mSelectedEntity.addComponent<CircleColliderComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!mSelectedEntity.hasComponent<TilemapComponent>())
			{
				if (ImGui::MenuItem("Tilemap"))
					ModalManager::Open<MapSpecModal>("MapSpecModal", mSelectedEntity);
			}

			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();

		if (mSelectedEntity.hasComponent<NodeComponent>())
		{
			std::string noParent = "None";
			std::unordered_map<std::string, Entity> possibleParents;

			// Create map of possible 
			mContext->mRegistry.group<TagComponent>(entt::get<IDComponent>).each([&](auto entityID, auto& tc, auto& idc) {
				if (mSelectedEntity != entityID)
				{
					possibleParents.emplace(tc.tag + "    (ID = " + idc.id.to_string() + ")", Entity{entityID, mContext});
				}
			});

			std::string currentParentString = "None";
			Entity parent(mSelectedEntity.getParent(), mContext);
			if (parent)
				currentParentString = parent.getComponent<TagComponent>().tag + "    (ID =" + parent.getUUID().to_string() + ")";

			if (ImGui::BeginCombo("Parent", currentParentString.c_str()))
			{
				// Display "None" at the top of the list
				bool clear = currentParentString == noParent;
				if (ImGui::Selectable(noParent.c_str(), clear))
					if (mSelectedEntity.setParent(Entity()))
						currentParentString = noParent;

				for (auto [name, parentEnt] : possibleParents)
				{
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

				float orthoNear = camera.getOrthographicNearClip();
				if (ImGui::DragFloat("Near", &orthoNear))
					camera.setOrthographicNearClip(orthoNear);

				float orthoFar = camera.getOrthographicFarClip();
				if (ImGui::DragFloat("Far", &orthoFar))
					camera.setOrthographicFarClip(orthoFar);

				ImGui::Checkbox("Fixed Aspect Ratio", &component.fixedAspectRatio);
			}
		});

		DrawComponent<SpriteRendererComponent>("Sprite Renderer", mSelectedEntity, [&](auto& component)
		{
			int layerVal = component.layer;
			if (ImGui::InputInt("Layer", &layerVal))
			{
				if (layerVal < 0) layerVal = 0;
				if (layerVal > component.MaxLayers) layerVal = component.MaxLayers;

				if (layerVal != component.layer)
				{
					component.layer = layerVal;
					mSelectedEntity.getComponent<TransformComponent>().translation.z = component.getNLayer();
				}
			}

			ImGui::ColorEdit4("Colour", glm::value_ptr(component.colour));

			std::string texLabel;
			switch (component.type)
			{
			case SpriteRendererComponent::TexType::None:
				texLabel = "None";
				break;

			case SpriteRendererComponent::TexType::Texture:
				texLabel = "Texture2D";
				break;

			case SpriteRendererComponent::TexType::SubTexture:
				texLabel = "SubTexture2D";
				break;
			}

			if (ImGui::BeginCombo("Texture Type", texLabel.c_str()))
			{
				for (const auto& [label, type] : mTexTypes)
				{
					bool isSelected = component.type == type;

					if (ImGui::Selectable(label.c_str(), isSelected))
					{
						component.type = type;
						switch (type)
						{
						case SpriteRendererComponent::TexType::None:       component.texture = SpriteRendererComponent::NoTex(); break;
						case SpriteRendererComponent::TexType::Texture:    component.texture = mNoTex; break;
						case SpriteRendererComponent::TexType::SubTexture: component.texture = SubTexture2D::Create(mNoTex, "NoTex"); break;
						}
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			auto viewportPanelWidth = ImGui::GetContentRegionAvail();
			ImGui::Text("Texture");
			ImTextureID tex = (ImTextureID)(intptr_t)(mTexture ? mTexture->getColourAttachmentRendererID() : mNoTex->getRendererID());
			ImGui::Image(tex, { viewportPanelWidth.x - 15.0f, 100.0f }, { 0, 1 }, { 1, 0 });
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const FS_CHAR_TYPE* path = (const FS_CHAR_TYPE*)payload->Data;
					std::filesystem::path texturePath = std::filesystem::path(gAssetPath) / path;

					if (std::regex_match(texturePath.extension().string(), Texture2D::GetSuppTypes()))
					{
						component.type = SpriteRendererComponent::TexType::Texture;
						component.texture = Texture2D::Create(texturePath.string());
					}
				}
				ImGui::EndDragDropTarget();
			}
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SPRITE_SHEET_ITEM"))
				{
					SpriteSheetData& data = *Cast<SpriteSheetData>(payload->Data);

					component.type = SpriteRendererComponent::TexType::SubTexture;
					component.texture = AssetManager::Get<Texture2DSheet>(data.sheetName)->getSubTex(data.subTexName);
				}
				ImGui::EndDragDropTarget();
			}
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_MANAGER_ITEM"))
				{
					const std::string& key = *Cast<std::string>(payload->Data);

					switch (component.type)
					{
					case SpriteRendererComponent::TexType::None:
						ImGui::OpenPopup("InvalidAsset");
						break;

					case SpriteRendererComponent::TexType::Texture:
					{
						Ref<Texture2D> tex = AssetManager::Get<Texture2D>(key);
						if (tex) 
							component.texture = tex; 
						else 
							ImGui::OpenPopup("InvalidAsset");
						break;
					}

					case SpriteRendererComponent::TexType::SubTexture:
						Ref<SubTexture2D> tex = AssetManager::Get<SubTexture2D>(key);
						if (tex)
							component.texture = tex;
						else
							ImGui::OpenPopup("InvalidAsset");
						break;
					}

				}
				ImGui::EndDragDropTarget();
			}

			ImGui::DragFloat("Tiling Factor", &component.tilingFactor, 0.1f, 0.0f, 100.0f);
		});

		DrawComponent<CircleRendererComponent>("Circle Renderer", mSelectedEntity, [&](auto& component)
		{
			int layerVal = component.layer;
			if (ImGui::InputInt("Layer", &layerVal))
			{
				if (layerVal < 0) layerVal = 0;
				if (layerVal > component.MaxLayers) layerVal = component.MaxLayers;

				if (layerVal != component.layer)
				{
					component.layer = layerVal;
					mSelectedEntity.getComponent<TransformComponent>().translation.z = component.getNLayer();
				}
			}

			ImGui::ColorEdit4("Colour", glm::value_ptr(component.colour));
			ImGui::DragFloat("Thickness", &component.thickness, 0.025f, 0.0f, 1.0f);
		});

		DrawComponent<RigidBodyComponent>("Rigid Body", mSelectedEntity, [&](auto& component)
		{
			const char* bodyTypeStrings[] = { "Static", "Dynamic", "Kinematic"};
			const char* currentBodyTypeString = bodyTypeStrings[(int)component.type];
			if (ImGui::BeginCombo("Type", currentBodyTypeString))
			{
				for (int i = 0; i < 3; i++)
				{
					bool isSelected = currentBodyTypeString == bodyTypeStrings[i];

					if (ImGui::Selectable(bodyTypeStrings[i], isSelected))
					{
						currentBodyTypeString = bodyTypeStrings[i];
						component.type = (RigidBodyComponent::BodyType)i;
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			ImGui::Checkbox("Fixed Rotation", &component.fixedRotation);
		});

		DrawComponent<BoxColliderComponent>("Box Collider", mSelectedEntity, [&](auto& component)
		{
			ImGui::DragFloat2("Half Extents", glm::value_ptr(component.halfExtents), 0.01f, 0.0f, 100.0f);
			ImGui::DragFloat2("Offset", glm::value_ptr(component.offset), 0.01f, 0.0f, 100.0f);
			ImGui::DragFloat("Friction", &component.friction, 0.01f, 0.0f, 100.0f);
			ImGui::DragFloat("Density", &component.density, 0.01f, 0.0f, 100.0f);
			ImGui::DragFloat("Restitution", &component.restitution, 0.01f, 0.0f, 100.0f);
			ImGui::DragFloat("Restitution Threshold", &component.restitutionThreshold, 0.01f, 0.0f, 100.0f);
		});

		DrawComponent<CircleColliderComponent>("Circle Collider", mSelectedEntity, [&](auto& component)
		{
			ImGui::DragFloat("Radius", &component.radius, 0.01f, 0.0f, 100.0f);
			ImGui::DragFloat2("Offset", glm::value_ptr(component.offset), 0.01f, 0.0f, 100.0f);
			ImGui::DragFloat("Friction", &component.friction, 0.01f, 0.0f, 100.0f);
			ImGui::DragFloat("Density", &component.density, 0.01f, 0.0f, 100.0f);
			ImGui::DragFloat("Restitution", &component.restitution, 0.01f, 0.0f, 100.0f);
			ImGui::DragFloat("Restitution Threshold", &component.restitutionThreshold, 0.01f, 0.0f, 100.0f);
		});

		DrawComponent<TilemapComponent>("Tilemap", mSelectedEntity, [&](auto& component)
		{
			auto viewportPanelWidth = ImGui::GetContentRegionAvail();
			ImGui::Text("Texture");
			ImGui::Image((ImTextureID)(intptr_t)component.tilemap->getFB()->getColourAttachmentRendererID(), {viewportPanelWidth.x - 15.0f, 100.0f}, {0, 1}, {1, 0});
		});

	}

	void ScenePanel::AssetTypeWarning()
	{
		if (!ImGui::BeginPopupModal("InvalidAsset", nullptr, ImGuiPopupFlags_None)) return;

		ImGui::Text("This Asset Type does not match the Entity texture type");
		ImGui::NewLine();

		if (ImGui::Button("OK"))
			ImGui::CloseCurrentPopup();

		ImGui::EndPopup();
	}

}