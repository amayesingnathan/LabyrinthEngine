#include "Lpch.h"
#include "EntityPanel.h"

#include <Labyrinth/Editor/EditorResources.h>
#include <Labyrinth/Editor/SelectionManager.h>
#include <Labyrinth/ImGui/ImGuiWidgets.h>
#include <Labyrinth/Renderer/Renderer.h>
#include <Labyrinth/Scripting/ScriptEngine.h>
#include <Labyrinth/Scripting/ScriptCache.h>
#include <Labyrinth/Tools/EnumUtils.h>

#include "SpriteSheetPanel.h"

namespace Laby {

	using ParentEntityEntry = ComboEntry<Entity>;
	using ScriptClassEntry = ComboEntry<Ref<ScriptClass>>;

	using TexTypeEntry = ComboEntry<SpriteRendererComponent::TexType>;
	static constexpr TexTypeEntry sTextureTypes[3] =
	{
		{ "Colour",			SpriteRendererComponent::TexType::None },
		{ "Texture2D",		SpriteRendererComponent::TexType::Texture },
		{ "SubTexture2D",	SpriteRendererComponent::TexType::SubTexture }
	};

	using CameraProjectionEntry = ComboEntry<SceneCamera::ProjectionType>;
	static constexpr CameraProjectionEntry sCameraProjections[2] =
	{
		{ "Perspective",  SceneCamera::ProjectionType::Perspective },
		{ "Orthographic", SceneCamera::ProjectionType::Orthographic }
	};

	using BodyTypeEntry = ComboEntry<RigidBodyComponent::BodyType>;
	static constexpr BodyTypeEntry sBodyTypes[3] =
	{
		{ "Static",		RigidBodyComponent::BodyType::Static },
		{ "Kinematic",	RigidBodyComponent::BodyType::Kinematic },
		{ "Dynamic",	RigidBodyComponent::BodyType::Dynamic }
	};

	EntityPanel::EntityPanel(const Ref<Scene>& scene)
		: mContext(scene)
	{
	}

	void EntityPanel::onImGuiRender()
	{
		if (!mSelectedEntity)
			return;

		DrawComponents();
	}

	void EntityPanel::onSelectionChange()
	{
		const auto& selections = SelectionManager::GetSelections(SelectionDomain::Scene);
		mSelectedEntity = selections.size() != 0 ? mContext->findEntity(selections[0]) : Entity{};

		if (mSelectedEntity && mPreviousEntity != mSelectedEntity)
			mPreviousEntity = mSelectedEntity;
	}

	void EntityPanel::DrawComponents()
	{
		if (mPreviousEntity)
		{
			Widgets::Button("<--", [&]() 
			{
				Entity holdPrevious = mPreviousEntity;
				Entity holdSelected = mSelectedEntity;

				SelectionManager::DeselectAll(SelectionDomain::Scene);
				SelectionManager::Select(SelectionDomain::Scene, holdPrevious.getUUID());
			});
		}

		Widgets::SameLine(ImGuiUtils::WindowWidth() - 72.0f);

		Widgets::Button("Destroy", [&]()
		{
			mSelectedEntity.destroy();
			SelectionManager::DeselectAll(SelectionDomain::Scene);
		});

		if (!mSelectedEntity)
			return; // In case of deletion

		if (mSelectedEntity.hasComponent<TagComponent>())
			Widgets::StringEdit("##Tag", mSelectedEntity.getTag());

		Widgets::SameLine();

		Widgets::Button("Add Component", []()
		{
			Widgets::OpenPopup("AddComponent");
		});

		UI::PopUp* addComponentPopup = Widgets::BeginPopup("AddComponentPopup");
		DrawAddComponentEntry<CameraComponent>(addComponentPopup, "Camera");
		DrawAddComponentEntry<SpriteRendererComponent>(addComponentPopup, "Sprite Renderer");
		DrawAddComponentEntry<CircleRendererComponent>(addComponentPopup, "Circle Renderer");
		DrawAddComponentEntry<RigidBodyComponent>(addComponentPopup, "Rigid Body");
		DrawAddComponentEntry<BoxColliderComponent>(addComponentPopup, "Box Collider");
		DrawAddComponentEntry<CircleColliderComponent>(addComponentPopup, "Circle Collider");
		DrawAddComponentEntry<ChildControllerComponent>(addComponentPopup, "Child Controller");
		DrawAddComponentEntry<ScriptComponent>(addComponentPopup, "Script");
		DrawAddComponentEntry<TilemapComponent>(addComponentPopup, "Tilemap Controller");
		Widgets::EndPopup(addComponentPopup);

		if (mSelectedEntity.hasComponent<NodeComponent>())
		{
			std::vector<ParentEntityEntry> comboEntries;

			auto view = mContext->mRegistry.view<TagComponent, IDComponent>();
			comboEntries.reserve(view.size_hint() + 1);
			comboEntries.emplace_back("None", Entity{});

			// Create map of possible parents
			view.each([&](auto entityID, auto& tc, auto& idc)
			{
				if (mSelectedEntity != entityID)
					comboEntries.emplace_back(fmt::format("{}\rID = ({})", tc.tag, idc.id.to_string()), Entity{entityID, mContext});
			});

			std::sort(comboEntries.begin() + 1, comboEntries.end(), [](const ParentEntityEntry& u, const ParentEntityEntry& v) { return u.value.getUUID() < v.value.getUUID(); });

			Entity parent = mSelectedEntity.getParent();
			std::string currentParentString = parent ? fmt::format("{}\tID = ({})", parent.getComponent<TagComponent>().tag, parent.getUUID()) : "None";

			Widgets::Combobox("Parent", currentParentString.c_str(), mSelectedEntity, comboEntries.data(), comboEntries.size());
		}

		Widgets::Component<TransformComponent>("Transform", mSelectedEntity, [](auto& component)
		{
			Widgets::Vector3Edit("Translation", component.translation);
			glm::vec3 rotation = glm::degrees(component.rotation);
			Widgets::Vector3Edit("Rotation", rotation);
			component.rotation = glm::radians(rotation);
			Widgets::Vector3Edit("Scale", component.scale, 1.0f);
		});

		Widgets::Component<CameraComponent>("Camera", mSelectedEntity, [&](auto& component)
		{
			auto& camera = component.camera;
			Widgets::Checkbox("Primary", component.primary, [&]()
			{
				//Ensure only one camera can be primary at once
				if (!component.primary)
					return;

				mContext->mRegistry.view<CameraComponent>().each([&](auto entity, auto& component)
				{
					if (mSelectedEntity == entity)
						return;

					component.primary = false;
				});
			});

			const SceneCamera::ProjectionType& projectionType = camera.getProjectionType();
			Widgets::Combobox("Projection", Enum::ToString(projectionType), projectionType, sCameraProjections, 2,
				[&](std::string_view, SceneCamera::ProjectionType projType) { camera.setProjectionType(projType); });

			switch (projectionType)
			{
			case SceneCamera::ProjectionType::Perspective:
			{
				Widgets::FloatEdit("Vertical FOV", glm::degrees(camera.getPerspectiveVerticalFOV()), 
					[&](f32 var) { camera.setPerspectiveVerticalFOV(glm::radians(var)); });

				Widgets::FloatEdit("Near", camera.getPerspectiveNearClip(),
					[&](f32 var) { camera.setPerspectiveNearClip(var); });

				Widgets::FloatEdit("Far", camera.getPerspectiveFarClip(),
					[&](f32 var) { camera.setPerspectiveFarClip(var); });
				break;
			}
			case SceneCamera::ProjectionType::Orthographic:
			{
				Widgets::FloatEdit("Size", camera.getOrthographicSize(),
					[&](f32 var) { camera.setOrthographicSize(var); });

				Widgets::FloatEdit("Near", camera.getPerspectiveNearClip(),
					[&](f32 var) { camera.setPerspectiveNearClip(var); });

				Widgets::FloatEdit("Far", camera.getPerspectiveFarClip(),
					[&](f32 var) { camera.setPerspectiveFarClip(var); });

				Widgets::Checkbox("Fixed Aspect Ratio", component.fixedAspectRatio);
				break;
			}
			}
		});

		Widgets::Component<SpriteRendererComponent>("Sprite Renderer", mSelectedEntity, [&](auto& component)
		{
			Widgets::UIntEdit("Layer", component.layer);
			Widgets::ColourEdit("Colour", component.colour);
			Widgets::Combobox("Texture Type", Enum::ToString(component.type), component.type, sTextureTypes, 3);

			Ref<IRenderable> tex = EditorResources::NoTexture;
			switch (component.type)
			{
			case SpriteRendererComponent::TexType::Texture:
				tex = AssetManager::GetAsset<Texture2D>(component.handle);
				break;
			case SpriteRendererComponent::TexType::SubTexture:
				tex = AssetManager::GetAsset<SubTexture2D>(component.handle);
				break;
			}

			Widgets::Label("Texture");
			Widgets::Image(tex, glm::vec2{ ImGuiUtils::AvailableRegion().x - 15.0f, 100.0f });
			Widgets::AddDragDropTarget<fs::path>("CONTENT_BROWSER_ITEM", [&](const fs::path& var)
			{
				fs::path texturePath = Project::GetAssetDirectory() / var;
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
			});
			Widgets::AddDragDropTarget<SpriteSheetData>("SPRITE_SHEET_ITEM", [&](const SpriteSheetData& var)
			{
				component.type = SpriteRendererComponent::TexType::SubTexture;
				component.handle = var.currentSubTex->handle;
			});
			Widgets::FloatEdit("Tiling Factor", component.tilingFactor, 0.1f, 0.0f, 100.0f);
		});

		Widgets::Component<CircleRendererComponent>("Circle Renderer", mSelectedEntity, [&](auto& component)
		{
			Widgets::UIntEdit("Layer", component.layer);
			Widgets::ColourEdit("Colour", component.colour);
			Widgets::FloatEdit("Thickness", component.thickness);
		});

		Widgets::Component<RigidBodyComponent>("Rigid Body", mSelectedEntity, [&](auto& component)
		{
			Widgets::Combobox("Type", Enum::ToString(component.type), component.type, sBodyTypes, 3);
			Widgets::Checkbox("Fixed Rotation", component.fixedRotation);
			Widgets::FloatEdit("Mass", component.mass, 0.01f, 0.0f, 100.0f);
			Widgets::FloatEdit("Linear Drag", component.linearDrag, 0.01f, 0.0f, 100.0f);
			Widgets::FloatEdit("Angular Drag", component.angularDrag, 0.01f, 0.0f, 100.0f);
			Widgets::FloatEdit("Gravity Scale", component.gravityScale, 0.01f, 0.0f, 100.0f);
		});

		Widgets::Component<BoxColliderComponent>("Box Collider", mSelectedEntity, [&](auto& component)
		{
			Widgets::Vector2Edit("Half Extents", component.halfExtents);
			Widgets::Vector2Edit("Offset", component.offset);
			Widgets::FloatEdit("Friction", component.friction, 0.01f, 0.0f, 100.0f);
			Widgets::FloatEdit("Density", component.density, 0.01f, 0.0f, 100.0f);
			Widgets::FloatEdit("Restitution", component.restitution, 0.01f, 0.0f, 100.0f);
			Widgets::FloatEdit("Restitution Threshold", component.restitutionThreshold, 0.01f, 0.0f, 100.0f);
		});

		Widgets::Component<CircleColliderComponent>("Circle Collider", mSelectedEntity, [&](auto& component)
		{
			Widgets::FloatEdit("Radius", component.radius);
			Widgets::Vector2Edit("Offset", component.offset);
			Widgets::FloatEdit("Friction", component.friction, 0.01f, 0.0f, 100.0f);
			Widgets::FloatEdit("Density", component.density, 0.01f, 0.0f, 100.0f);
			Widgets::FloatEdit("Restitution", component.restitution, 0.01f, 0.0f, 100.0f);
			Widgets::FloatEdit("Restitution Threshold", component.restitutionThreshold, 0.01f, 0.0f, 100.0f);
		});

		Widgets::Component<ScriptComponent>("Script", mSelectedEntity, [&](auto& component)
		{
			std::vector<ScriptClassEntry> comboEntries;
			for (const auto& [key, klass] : ScriptEngine::GetAppClasses())
				comboEntries.emplace_back(key, klass);

			UUID id = mSelectedEntity.getUUID();
			Ref<ScriptClass> scriptClass = ScriptEngine::GetAppClass(component.className.data());
			Widgets::Combobox("Class", component.className, scriptClass, comboEntries.data(), comboEntries.size(), [&](std::string_view name, Ref<ScriptClass> klass) 
			{				
				ScriptCache::RemoveEntity(id);
				component.className = name;
				ScriptCache::RegisterEntity(id, klass);
			});

			if (component.instance)
				DrawScriptInstanceFields(mSelectedEntity);
			else
				DrawScriptClassFields(mSelectedEntity);
		});
	}

	void EntityPanel::DrawChildControllerElement(const std::string& name, glm::vec3& componentElement, glm::vec3& displayElement, glm::vec3& lastDisplay, float min, float max, ImGuiSliderFlags flags)
	{
	}

	void EntityPanel::DrawScriptClassFields(Entity entity)
	{
		UUID entID = entity.getUUID();
		for (auto& [name, fieldValue] : ScriptCache::GetFields(entID))
		{
			switch (fieldValue.type)
			{
			case ScriptFieldType::Boolean:
				Widgets::Checkbox(name.data(), ScriptCache::GetFieldValue<bool>(entID, name));
				break;
			case ScriptFieldType::Int8:
				Widgets::IntEdit(name.data(), ScriptCache::GetFieldValue<i8>(entID, name));
				break;
			case ScriptFieldType::Int16:
				Widgets::IntEdit(name.data(), ScriptCache::GetFieldValue<i16>(entID, name));
				break;
			case ScriptFieldType::Int32:
				Widgets::IntEdit(name.data(), ScriptCache::GetFieldValue<i32>(entID, name));
				break;
			case ScriptFieldType::Int64:
				Widgets::IntEdit(name.data(), ScriptCache::GetFieldValue<i64>(entID, name));
				break;
			case ScriptFieldType::UInt8:
				Widgets::UIntEdit(name.data(), ScriptCache::GetFieldValue<u8>(entID, name));
				break;
			case ScriptFieldType::UInt16:
				Widgets::UIntEdit(name.data(), ScriptCache::GetFieldValue<u16>(entID, name));
				break;
			case ScriptFieldType::UInt32:
				Widgets::UIntEdit(name.data(), ScriptCache::GetFieldValue<u32>(entID, name));
				break;
			case ScriptFieldType::UInt64:
				Widgets::UIntEdit(name.data(), ScriptCache::GetFieldValue<u64>(entID, name));
				break;
			case ScriptFieldType::Float:
				Widgets::FloatEdit(name.data(), ScriptCache::GetFieldValue<f32>(entID, name));
				break;
			case ScriptFieldType::Double:
				Widgets::DoubleEdit(name.data(), ScriptCache::GetFieldValue<f64>(entID, name));
				break;
			case ScriptFieldType::Vector2:
				Widgets::Vector2Edit(name.data(), ScriptCache::GetFieldValue<glm::vec2>(entID, name));
				break;
			case ScriptFieldType::Vector3:
				Widgets::Vector3Edit(name.data(), ScriptCache::GetFieldValue<glm::vec3>(entID, name));
				break;
			case ScriptFieldType::Vector4:
				Widgets::Vector4Edit(name.data(), ScriptCache::GetFieldValue<glm::vec4>(entID, name));
				break;
			case ScriptFieldType::Entity:
			{
				UUID& fieldEntity = ScriptCache::GetFieldValue<UUID>(entID, name);
				std::string_view entName = fieldEntity ? mContext->findEntity(fieldEntity).getTag() : "No Entity";
				Widgets::Button(entName.data());
				Widgets::AddDragDropTarget<UUID>("ENTITY_ITEM", [&](const UUID& entity)
				{
					fieldEntity = entity;
				});
				Widgets::SameLine();
				Widgets::Label(name);
				break;
			}
			}
		}
	}

	void EntityPanel::DrawScriptInstanceFields(Entity entity)
	{
		auto& script = entity.getComponent<ScriptComponent>();

		const auto& fields = script.instance->getScriptClass()->getFields();
		for (const auto& [type, name, field] : fields)
		{
			switch (type)
			{
			case ScriptFieldType::Boolean:
			{
				bool data;
				script.instance->getFieldValue(name, data);
				Widgets::Checkbox(name, data, [&]() { script.instance->setFieldValue(name, data); });
				break;
			}
			case ScriptFieldType::Int8:
			{
				i8 data;
				script.instance->getFieldValue(name, data);
				Widgets::IntEdit<i8>(name, data, [&](i8 val) { script.instance->setFieldValue(name, val); });
				break;
			}
			case ScriptFieldType::Int16:
			{
				i16 data;
				script.instance->getFieldValue(name, data);
				Widgets::IntEdit<i16>(name, data, [&](i16 val) { script.instance->setFieldValue(name, val); });
				break;
			}
			case ScriptFieldType::Int32:
			{
				i32 data;
				script.instance->getFieldValue(name, data);
				Widgets::IntEdit<i32>(name, data, [&](i32 val) { script.instance->setFieldValue(name, val); });
				break;
			}
			case ScriptFieldType::Int64:
			{
				i64 data;
				script.instance->getFieldValue(name, data);
				Widgets::IntEdit<i64>(name, data, [&](i64 val) { script.instance->setFieldValue(name, val); });
				break;
			}
			case ScriptFieldType::UInt8:
			{
				u8 data;
				script.instance->getFieldValue(name, data);
				Widgets::UIntEdit<u8>(name, data, [&](u8 val) { script.instance->setFieldValue(name, val); });
				break;
			}
			case ScriptFieldType::UInt16:
			{
				u16 data;
				script.instance->getFieldValue(name, data);
				Widgets::UIntEdit<u16>(name, data, [&](u16 val) { script.instance->setFieldValue(name, val); });
				break;
			}
			case ScriptFieldType::UInt32:
			{
				u32 data;
				script.instance->getFieldValue(name, data);
				Widgets::UIntEdit<u32>(name, data, [&](u32 val) { script.instance->setFieldValue(name, val); });
				break;
			}
			case ScriptFieldType::UInt64:
			{
				u64 data;
				script.instance->getFieldValue(name, data);
				Widgets::UIntEdit<u64>(name, data, [&](u64 val) { script.instance->setFieldValue(name, val); });
				break;
			}
			case ScriptFieldType::Float:
			{
				f32 data;
				script.instance->getFieldValue(name, data);
				Widgets::FloatEdit(name, data, [&](f32 val) { script.instance->setFieldValue(name, val); });
				break;
			}
			case ScriptFieldType::Double:
			{
				f64 data;
				script.instance->getFieldValue(name, data);
				Widgets::DoubleEdit(name, data, [&](f64 val) { script.instance->setFieldValue(name, val); });
				break;
			}
			case ScriptFieldType::Vector2:
			{
				glm::vec2 data;
				script.instance->getFieldValue(name, data);
				Widgets::Vector2Edit(name, data, [&](const glm::vec2& val) { script.instance->setFieldValue(name, val); });
				break;
			}
			case ScriptFieldType::Vector3:
			{
				glm::vec3 data;
				script.instance->getFieldValue(name, data);
				Widgets::Vector3Edit(name, data, [&](const glm::vec3& val) { script.instance->setFieldValue(name, val); });
				break;
			}
			case ScriptFieldType::Vector4:
			{
				glm::vec4 data;
				script.instance->getFieldValue(name, data);
				Widgets::Vector4Edit(name, data, [&](const glm::vec4& val) { script.instance->setFieldValue(name, val); });
				break;
			}
			case ScriptFieldType::Entity:
			{
				UUID data = 0;
				script.instance->getEntityFieldValue(name, data);
				std::string_view entName = data ? mContext->findEntity(data).getTag() : "No Entity";

				Widgets::Button(entName.data());
				Widgets::SameLine();
				Widgets::Label(name);
				break;
			}
			}
		}
	}
}
