#include "ScenePanel.h"

#include "SpriteSheetData.h"
#include "../EditorLayer.h"
#include "../Modals/BodySpecModal.h"
#include "../Modals/NewMapModal.h"

#include <Labyrinth/Assets/AssetManager.h>
#include <Labyrinth/Editor/EditorResources.h>
#include <Labyrinth/Editor/ModalManager.h>
#include <Labyrinth/Editor/SelectionManager.h>
#include <Labyrinth/ImGui/ImGuiUtils.h>
#include <Labyrinth/IO/Input.h>
#include <Labyrinth/Renderer/Renderer2D.h>
#include <Labyrinth/Renderer/RenderCommand.h>

#include <Labyrinth/Scripting/ScriptEngine.h>

#include <glm/gtc/type_ptr.hpp>

namespace Labyrinth {

	ScenePanel::ScenePanel()
	{
	}

	ScenePanel::ScenePanel(EditorData& options)
	{
		mEditorData = &options;
	}

	ScenePanel::ScenePanel(const Ref<Scene>& scene, EditorData& options)
	{
		mContext = scene;
		mEditorData = &options;
	}

	void ScenePanel::setContext(const Ref<Scene>& scene)
	{
		mContext = scene;
	}

	void ScenePanel::setContext(const Ref<Scene>& scene, EditorData& options)
	{
		mContext = scene;
		mEditorData = &options;
	}

	void ScenePanel::onUpdate(Timestep ts)
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
			Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 1.0f, 1.0f }, EditorResources::NoTexture);
			break;

		case SpriteRendererComponent::TexType::Texture:
		{
			Ref<Texture2D> tex = component.handle ? AssetManager::GetAsset<Texture2D>(component.handle) : EditorResources::NoTexture;
			if (!tex) tex = EditorResources::NoTexture;
			Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 1.0f, 1.0f }, tex);
		}
		break;

		case SpriteRendererComponent::TexType::SubTexture:
		{
			Ref<SubTexture2D> subtex = component.handle ? AssetManager::GetAsset<SubTexture2D>(component.handle) : EditorResources::NoSubTexture;
			if (!subtex) subtex = EditorResources::NoSubTexture;
			Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 1.0f, 1.0f }, subtex);
		}
		break;
		}

		Renderer2D::EndState();
		mTexture->unbind();
	}

	void ScenePanel::onImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		StaticBuffer<256> buffer(mContext->getName());
		if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			mContext->setName(buffer.string());

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
				ModalManager::Open<BodySpecModal>("BodySpecModal", ModalButtons::OKCancel, mContext);
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

	void ScenePanel::onSelectionChange()
	{
		if (mSelectedEntity && mPreviousEntity != mSelectedEntity)
			mPreviousEntity = mSelectedEntity;

		const auto& selections = SelectionManager::GetSelections(SelectionDomain::Scene);
		mSelectedEntity = mContext->findEntity(selections.size() != 0 ? selections[0] : 0);
	}

	void ScenePanel::DrawEntityNode(Entity entity)
	{
		std::string& tag = entity.getComponent<TagComponent>();
		auto& node = entity.getComponent<NodeComponent>();

		ImGuiTreeNodeFlags flags = ((mSelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

		bool opened = ImGui::TreeNodeEx((void*)(u64)(u32)entity, flags, tag.c_str());
		if (ImGui::IsItemClicked())
		{
			SelectionManager::DeselectAll(SelectionDomain::Scene);
			SelectionManager::Select(SelectionDomain::Scene, entity.getUUID());
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
				DrawEntityNode(mContext->findEntity(node.children[i]));
			ImGui::TreePop();
		}
		if (entityCreated)
			mContext->CreateEntity("Empty Entity");
		if (bodyCreated)
			ModalManager::Open<BodySpecModal>("BodySpecModal", ModalButtons::OKCancel, mContext);
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
		if (ImGui::Button("<--") && mPreviousEntity)
		{
			Entity holdPrevious = mPreviousEntity;
			Entity holdSelected = mSelectedEntity;

			SelectionManager::DeselectAll(SelectionDomain::Scene);
			SelectionManager::Select(SelectionDomain::Scene, holdPrevious.getUUID());
			mPreviousEntity = holdSelected;
		}

		ImGui::SameLine(ImGui::GetWindowWidth() - 72);

		if (ImGui::Button("Destroy"))
		{
			mSelectedEntity.destroy();
			SelectionManager::DeselectAll(SelectionDomain::Scene);
			return;
		}

		if (mSelectedEntity.hasComponent<TagComponent>())
		{
			std::string& tag = mSelectedEntity.getComponent<TagComponent>();

			StaticBuffer<256> buffer(tag);
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
				tag = buffer.string();
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		bool addTileMap = false;
		if (ImGui::BeginPopup("AddComponent"))
		{
			DrawAddComponentEntry<CameraComponent>("Camera");
			DrawAddComponentEntry<SpriteRendererComponent>("Sprite Renderer");
			DrawAddComponentEntry<CircleRendererComponent>("Circle Renderer");
			DrawAddComponentEntry<RigidBodyComponent>("Rigid Body");
			DrawAddComponentEntry<BoxColliderComponent>("Box Collider");
			DrawAddComponentEntry<CircleColliderComponent>("Circle Collider");
			DrawAddComponentEntry<ChildControllerComponent>("Child Controller");
			DrawAddComponentEntry<ScriptComponent>("Script");
			DrawAddComponentEntry<TilemapControllerComponent>("Tilemap Controller");

			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();

		if (mSelectedEntity.hasComponent<NodeComponent>())
		{
			std::string noParent = "None";
			std::unordered_map<std::string, Entity> possibleParents;

			// Create map of possible 
			mContext->mRegistry.group<TagComponent>(entt::get<IDComponent>).each([&](auto entityID, auto& tc, auto& idc) 
			{
				if (mSelectedEntity != entityID)
				{
					possibleParents.emplace(tc.tag + "    (ID = " + idc.id.to_string() + ")", Entity{entityID, mContext});
				}
			});

			std::string currentParentString = "None";
			Entity parent = mSelectedEntity.getParent();
			if (parent)
				currentParentString = fmt::format("{}\tID = ({})", parent.getComponent<TagComponent>().tag, parent.getUUID());

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
			EditorUI::Vec3Control("Translation", component.translation);
			glm::vec3 rotation = glm::degrees(component.rotation);
			EditorUI::Vec3Control("Rotation", rotation);
			component.rotation = glm::radians(rotation);
			EditorUI::Vec3Control("Scale", component.scale, 1.0f);
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
			const char* currentProjectionTypeString = projectionTypeStrings[(i32)camera.getProjectionType()];
			if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
			{
				for (i32 i = 0; i < 2; i++)
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
			i32 layerVal = component.layer;
			if (ImGui::InputInt("Layer", &layerVal))
			{
				if (layerVal < 0) layerVal = 0;
				if (layerVal > component.MaxLayers) layerVal = component.MaxLayers;

				if (layerVal != component.layer)
					component.layer = layerVal;
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
						component.handle = 0;
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			auto viewportPanelWidth = ImGui::GetContentRegionAvail();
			ImGui::Text("Texture");
			ImTextureID tex = (ImTextureID)(intptr_t)(mTexture ? mTexture->getColourAttachmentRendererID() : EditorResources::NoTexture->getRendererID());
			ImGui::Image(tex, { viewportPanelWidth.x - 15.0f, 100.0f }, { 0, 1 }, { 1, 0 });
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const FS_CHAR_TYPE* path = (const FS_CHAR_TYPE*)payload->Data;
					std::filesystem::path texturePath = Project::GetAssetDirectory() / path;
					std::string extension = texturePath.extension().string();

					if (AssetManager::IsExtensionValid(extension, AssetType::Texture))
					{
						component.type = SpriteRendererComponent::TexType::Texture;
						component.handle = AssetManager::GetAssetHandleFromPath(texturePath);
					}
					else if (AssetManager::IsExtensionValid(extension, AssetType::SubTexture))
					{
						component.type = SpriteRendererComponent::TexType::SubTexture;
						component.handle = AssetManager::GetAssetHandleFromPath(texturePath);
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
					component.handle = data.currentSubTex->handle;
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::DragFloat("Tiling Factor", &component.tilingFactor, 0.1f, 0.0f, 100.0f);
		});

		DrawComponent<CircleRendererComponent>("Circle Renderer", mSelectedEntity, [&](auto& component)
		{
			i32 layerVal = component.layer;
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
			const char* currentBodyTypeString = bodyTypeStrings[(i32)component.type];
			if (ImGui::BeginCombo("Type", currentBodyTypeString))
			{
				for (i32 i = 0; i < 3; i++)
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
			ImGui::DragFloat("Mass", &component.mass, 0.01f, 0.0f, 100.0f);
			ImGui::DragFloat("Linear Drag", &component.linearDrag, 0.01f, 0.0f, 100.0f);
			ImGui::DragFloat("Angular Drag", &component.angularDrag, 0.01f, 0.0f, 100.0f);
			ImGui::DragFloat("Gravity Scale", &component.gravityScale, 0.01f, 0.0f, 100.0f);
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

		DrawComponent<ChildControllerComponent>("Child Controller", mSelectedEntity, [&](auto& component)
		{
			static ChildControllerComponent sController, sLastController;

			DrawChildControllerElement("Translation", component.deltaTranslation, sController.deltaTranslation, sLastController.deltaTranslation, -10.0f, 10.0f, ImGuiSliderFlags_Logarithmic);
			DrawChildControllerElement("Rotation", component.deltaRotation, sController.deltaRotation, sLastController.deltaRotation, 0.0f, 360.f);
			DrawChildControllerElement("Scale", component.deltaScale, sController.deltaScale, sLastController.deltaScale, -5.0f, 5.0f, ImGuiSliderFlags_Logarithmic);
		});

		DrawComponent<ScriptComponent>("Script", mSelectedEntity, [&](auto& component)
		{
			if (ImGui::BeginCombo("Class", component.className.c_str()))
			{
				// Display "None" at the top of the list
				bool clear = component.className.empty();
				if (ImGui::Selectable("None", clear))
					component.className.clear();

				for (const auto& [key, klass] : ScriptEngine::GetAppClasses())
				{
					bool isSelected = component.className == key;

					if (ImGui::Selectable(key.c_str(), isSelected))
						component.className = key;

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}
		});

		DrawComponent<TilemapControllerComponent>("Tilemap", mSelectedEntity, [&, this](auto& component)
		{
			ImVec2 cursorPos = ImGui::GetCursorPos();
			auto imageSize = ImGui::GetContentRegionAvail();
			imageSize = { imageSize.x - 15.0f, 150.0f };

			Ref<Tilemap> tilemap = AssetManager::GetAsset<Tilemap>(component.tilemapHandle);
			ImTextureID tex = (ImTextureID)(intptr_t)(tilemap ? tilemap->getTex()->getColourAttachmentRendererID() : EditorResources::NoTexture->getRendererID());
			ImGui::Image(tex, imageSize, { 0, 1 }, { 1, 0 });

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TILEMAP_ITEM"))
				{
					AssetHandle tilemapHandle = *(AssetHandle*)payload->Data;
					component.tilemapHandle = tilemapHandle;
					mContext->reloadMaps();
				}
				ImGui::EndDragDropTarget();
			}

			if (!tilemap)
				return;

			ImGui::SetCursorPos(cursorPos);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, mHoveredButtonColour);
			EditorUI::GridControl(imageSize, tilemap->getWidth(), tilemap->getHeight(), [this, &component](const TilePos& pos, const ImVec2& elementSize)
			{
				std::string name = fmt::format("##SelectTile({}, {})", pos.x, pos.y);

				bool active = component.tileBehaviour.count(pos) != 0;
				ImVec4 buttonColour = active ? mActiveButtonColour : mDefaultButtonColour;
				ImGui::PushStyleColor(ImGuiCol_Button, buttonColour);
				if (ImGui::Button(name.c_str(), elementSize) && active)
				{
					SelectionManager::DeselectAll(SelectionDomain::Scene);
					SelectionManager::Select(SelectionDomain::Scene, component.tileBehaviour[pos]);
				}
				ImGui::PopStyleColor();
			});
			ImGui::PopStyleColor();
		});
	}

	void ScenePanel::DrawChildControllerElement(const std::string& name, glm::vec3& componentElement, glm::vec3& displayElement, glm::vec3& lastDisplay, float min, float max, ImGuiSliderFlags flags)
	{
		bool mousePressed = Input::IsMouseButtonPressed(Mouse::ButtonLeft);

		ImGui::SliderFloat3(name.c_str(), glm::value_ptr(displayElement), min, max, "%.2f", flags);

		glm::vec3 diff = displayElement - lastDisplay;
		if (diff != Zero::Vec3)
		{
			componentElement = diff;
			mContext->transformChildren();
		}
		else if (!mousePressed && displayElement != Zero::Vec3)
			displayElement = Zero::Vec3;

		lastDisplay = displayElement;
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