#include "Lpch.h"
#include "EntityPanel.h"

#include <Labyrinth/Assets/AssetManager.h>
#include <Labyrinth/Editor/EditorResources.h>
#include <Labyrinth/Editor/SelectionManager.h>
#include <Labyrinth/ImGui/ImGuiWidgets.h>
#include <Labyrinth/Renderer/Renderer.h>
#include <Labyrinth/Scripting/ScriptEngine.h>
#include <Labyrinth/Scripting/ScriptCache.h>
#include <Labyrinth/Tilemap/Tilemap.h>
#include <Labyrinth/Tools/EnumUtils.h>

#include "SpriteSheetPanel.h"

using imcpp::Widgets;
using imcpp::Utils;

namespace Laby {

	using ParentEntityEntry = imcpp::ComboEntry<Entity>;
	using ScriptClassEntry = imcpp::ComboEntry<Ref<ScriptClass>>;

	using TexTypeEntry = imcpp::ComboEntry<SpriteRendererComponent::TexType>;
	static constexpr std::array<TexTypeEntry, 4> sTextureTypes =
	{
		TexTypeEntry{ "Colour",			SpriteRendererComponent::TexType::None },
		TexTypeEntry{ "Texture2D",		SpriteRendererComponent::TexType::Texture },
		TexTypeEntry{ "SubTexture2D",	SpriteRendererComponent::TexType::SubTexture },
		TexTypeEntry{ "Tilemap",		SpriteRendererComponent::TexType::Tilemap }
	};

	using CameraProjectionEntry = imcpp::ComboEntry<SceneCamera::ProjectionType>;
	static constexpr std::array<CameraProjectionEntry, 2> sCameraProjections =
	{
		CameraProjectionEntry{ "Perspective",  SceneCamera::ProjectionType::Perspective },
		CameraProjectionEntry{ "Orthographic", SceneCamera::ProjectionType::Orthographic }
	};

	using BodyTypeEntry = imcpp::ComboEntry<RigidBodyComponent::BodyType>;
	static constexpr std::array<BodyTypeEntry, 3> sBodyTypes =
	{
		BodyTypeEntry{ "Static",		RigidBodyComponent::BodyType::Static },
		BodyTypeEntry{ "Kinematic",	RigidBodyComponent::BodyType::Kinematic },
		BodyTypeEntry{ "Dynamic",	RigidBodyComponent::BodyType::Dynamic }
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

		Widgets::SameLine(Utils::WindowWidth() - 72.0f);

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
			Widgets::OpenPopup("AddComponentPopup");
		});

		Widgets::BeginPopup("AddComponentPopup");
		DrawAddComponentEntry<CameraComponent>("Camera");
		DrawAddComponentEntry<SpriteRendererComponent>("Sprite Renderer");
		DrawAddComponentEntry<CircleRendererComponent>("Circle Renderer");
		DrawAddComponentEntry<RigidBodyComponent>("Rigid Body");
		DrawAddComponentEntry<BoxColliderComponent>("Box Collider");
		DrawAddComponentEntry<CircleColliderComponent>("Circle Collider");
		DrawAddComponentEntry<ChildControllerComponent>("Child Controller");
		DrawAddComponentEntry<ScriptComponent>("Script");
		DrawAddComponentEntry<TilemapComponent>("Tilemap");
		Widgets::EndPopup();

		if (mSelectedEntity.hasComponent<NodeComponent>())
		{
			std::vector<ParentEntityEntry> comboEntries;

			auto view = mContext->getEntitiesWith<TagComponent, IDComponent>();
			comboEntries.reserve(view.size_hint() + 1);
			comboEntries.emplace_back("None", Entity{});

			// Create map of possible parents
			view.each([&](auto entityID, auto& tc, auto& idc)
			{
				if (mSelectedEntity != entityID)
					comboEntries.emplace_back(fmt::format("{}\rID = ({})", tc.tag, idc.id.to_string()), Entity{entityID, mContext});
			});

			std::sort(comboEntries.begin(), comboEntries.end());

			Entity parent = mSelectedEntity.getParent();
			std::string currentParentString = parent ? fmt::format("{}\tID = ({})", parent.getComponent<TagComponent>().tag, parent.getUUID()) : "None";

			Widgets::Combobox<Entity>("Parent", currentParentString.c_str(), mSelectedEntity, comboEntries);
		}

		LabWidgets::Component<TransformComponent>("Transform", mSelectedEntity, [](auto& component)
		{
			Widgets::Vector3Edit("Translation", component.translation);
			glm::vec3 rotation = glm::degrees(component.rotation);
			Widgets::Vector3Edit("Rotation", rotation);
			component.rotation = glm::radians(rotation);
			Widgets::Vector3Edit("Scale", component.scale, 1.0f);
		});

		LabWidgets::Component<CameraComponent>("Camera", mSelectedEntity, [&](auto& component)
		{
			auto& camera = component.camera;
			Widgets::Checkbox("Primary", component.primary, [&]()
			{
				//Ensure only one camera can be primary at once
				if (!component.primary)
					return;

				mContext->getEntitiesWith<CameraComponent>().each([&](EntityID entity, auto& component)
				{
					if (mSelectedEntity == Entity{ entity, mContext })
						return;

					component.primary = false;
				});
			});

			const SceneCamera::ProjectionType& projectionType = camera.getProjectionType();
			Widgets::Combobox<SceneCamera::ProjectionType>("Projection", Enum::ToString(projectionType), projectionType, sCameraProjections,
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

		LabWidgets::Component<SpriteRendererComponent>("Sprite Renderer", mSelectedEntity, [&](auto& component)
		{
			Widgets::UIntEdit("Layer", component.layer);
			Widgets::ColourEdit("Colour", component.colour);
			Widgets::Combobox<SpriteRendererComponent::TexType>("Texture Type", Enum::ToString(component.type), component.type, sTextureTypes);

			Ref<IRenderable> tex = EditorResources::NoTexture;
			switch (component.type)
			{
			case SpriteRendererComponent::TexType::Texture:
				tex = AssetManager::GetAsset<Texture2D>(component.handle);
				break;
			case SpriteRendererComponent::TexType::SubTexture:
				tex = AssetManager::GetAsset<SubTexture2D>(component.handle);
				break;
			case SpriteRendererComponent::TexType::Tilemap:
				tex = AssetManager::GetAsset<Tilemap>(component.handle);
				break;
			}

			Widgets::Label("Texture");
			LabWidgets::Image(tex, glm::vec2{ Utils::AvailableRegion().x - 15.0f, 100.0f });
			Widgets::AddDragDropTarget<fs::path>("CONTENT_BROWSER_ITEM", [&](const fs::path& var)
			{
				std::string extension = var.extension().string();

				if (AssetManager::IsExtensionValid(extension, AssetType::Texture))
				{
					component.type = SpriteRendererComponent::TexType::Texture;
					component.handle = AssetManager::GetAssetHandleFromPath(var);
				}
				else if (AssetManager::IsExtensionValid(extension, AssetType::SubTexture))
				{
					component.type = SpriteRendererComponent::TexType::SubTexture;
					component.handle = AssetManager::GetAssetHandleFromPath(var);
				}
				else if (AssetManager::IsExtensionValid(extension, AssetType::Tilemap))
				{
					component.type = SpriteRendererComponent::TexType::Tilemap;
					component.handle = AssetManager::GetAssetHandleFromPath(var);
				}
			});
			Widgets::AddDragDropTarget<AssetHandle>("SPRITE_SHEET_ITEM", [&](const AssetHandle& var)
			{
				component.type = SpriteRendererComponent::TexType::SubTexture;
				component.handle = var;
			});
			Widgets::AddDragDropTarget<AssetHandle>("TILEMAP_ITEM", [&](const AssetHandle& var)
			{
				component.type = SpriteRendererComponent::TexType::Tilemap;
				component.handle = var;
			});
			Widgets::FloatEdit("Tiling Factor", component.tilingFactor, 0.1f, 0.0f, 100.0f);
		});

		LabWidgets::Component<CircleRendererComponent>("Circle Renderer", mSelectedEntity, [&](auto& component)
		{
			Widgets::UIntEdit("Layer", component.layer);
			Widgets::ColourEdit("Colour", component.colour);
			Widgets::FloatEdit("Thickness", component.thickness);
		});

		LabWidgets::Component<RigidBodyComponent>("Rigid Body", mSelectedEntity, [&](auto& component)
		{
			Widgets::Combobox<RigidBodyComponent::BodyType>("Type", Enum::ToString(component.type), component.type, sBodyTypes);
			Widgets::Checkbox("Fixed Rotation", component.fixedRotation);
			Widgets::FloatEdit("Mass", component.mass, 0.01f, 0.0f, 100.0f);
			Widgets::FloatEdit("Linear Drag", component.linearDrag, 0.01f, 0.0f, 100.0f);
			Widgets::FloatEdit("Angular Drag", component.angularDrag, 0.01f, 0.0f, 100.0f);
			Widgets::FloatEdit("Gravity Scale", component.gravityScale, 0.01f, 0.0f, 100.0f);
		});

		LabWidgets::Component<BoxColliderComponent>("Box Collider", mSelectedEntity, [&](auto& component)
		{
			Widgets::Vector2Edit("Half Extents", component.halfExtents);
			Widgets::Vector2Edit("Offset", component.offset);
			Widgets::FloatEdit("Friction", component.friction, 0.01f, 0.0f, 100.0f);
			Widgets::FloatEdit("Density", component.density, 0.01f, 0.0f, 100.0f);
			Widgets::FloatEdit("Restitution", component.restitution, 0.01f, 0.0f, 100.0f);
			Widgets::FloatEdit("Restitution Threshold", component.restitutionThreshold, 0.01f, 0.0f, 100.0f);
		});

		LabWidgets::Component<CircleColliderComponent>("Circle Collider", mSelectedEntity, [&](auto& component)
		{
			Widgets::FloatEdit("Radius", component.radius);
			Widgets::Vector2Edit("Offset", component.offset);
			Widgets::FloatEdit("Friction", component.friction, 0.01f, 0.0f, 100.0f);
			Widgets::FloatEdit("Density", component.density, 0.01f, 0.0f, 100.0f);
			Widgets::FloatEdit("Restitution", component.restitution, 0.01f, 0.0f, 100.0f);
			Widgets::FloatEdit("Restitution Threshold", component.restitutionThreshold, 0.01f, 0.0f, 100.0f);
		});

		LabWidgets::Component<ScriptComponent>("Script", mSelectedEntity, [&](auto& component)
		{
			std::vector<ScriptClassEntry> comboEntries;
			for (const auto& [key, klass] : ScriptEngine::GetAppClasses())
				comboEntries.emplace_back(key, klass);

			UUID id = mSelectedEntity.getUUID();
			Ref<ScriptClass> scriptClass = ScriptEngine::GetAppClass(component.className.data());
			Widgets::Combobox<Ref<ScriptClass>>("Class", component.className, scriptClass, comboEntries, [&](std::string_view name, Ref<ScriptClass> klass) 
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

		LabWidgets::Component<TilemapComponent>("Tilemap", mSelectedEntity, [&](auto& component)
		{
			glm::vec2 cursorPos = Utils::CursorPos<glm::vec2>();
			auto imageSize = Utils::AvailableRegion<glm::vec2>();
			imageSize = { imageSize.x - 15.0f, 150.0f };
			Ref<Tilemap> tilemap = AssetManager::GetAsset<Tilemap>(component.mapHandle);
			LabWidgets::Image(tilemap, imageSize);
			Widgets::AddDragDropTarget<AssetHandle>("TILEMAP_ITEM", [&](const AssetHandle& map) 
			{
				component.mapHandle = map; 
				if (mSelectedEntity.hasComponent<SpriteRendererComponent>())
				{
					auto& sprite = mSelectedEntity.getComponent<SpriteRendererComponent>();
					sprite.type = SpriteRendererComponent::TexType::Tilemap;
					sprite.handle = map;	
				} 
			});

			if (!tilemap)
				return;

			Utils::PushStyleColour(ImGuiCol_ButtonHovered, Utils::ToImVec<ImVec4>(EditorResources::HoveredColour));
			Widgets::GridControl<glm::vec2>(cursorPos, imageSize, tilemap->getWidth(), tilemap->getHeight(), [this, tilemap](u32 x, u32 y, const glm::vec2& elementSize)
			{
				const auto& behaviour = tilemap->getTileBehaviour({ x, y });
				bool hasScript = !behaviour.script.empty();
				const glm::vec4& buttonColour = hasScript ? EditorResources::HighlightedColour : EditorResources::ClearColour;
				Utils::PushStyleColour(ImGuiCol_Button, Utils::ToImVec<ImVec4>(buttonColour));

				Widgets::Button(elementSize, std::format("##SelectTile({}, {})", x, y), [&, this]()
				{
					Entity scriptEntity = mSelectedEntity
						.findChild("Scripts")
						.findChild(std::format("{}-Script({}, {} - {})", tilemap->getName(), x, y, behaviour.script));

					if (!scriptEntity)
						return;

					SelectionManager::DeselectAll(SelectionDomain::Scene);
					SelectionManager::Select(SelectionDomain::Scene, scriptEntity.getUUID());
				});

				Utils::PopStyleColour();
			});
			Utils::PopStyleColour();
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
				Widgets::Vector2Edit(name, data, [&](const ImVec2& val) { script.instance->setFieldValue(name, Utils::FromImVec<glm::vec2>(val)); });
				break;
			}
			case ScriptFieldType::Vector3:
			{
				glm::vec3 data;
				script.instance->getFieldValue(name, data);
				Widgets::Vector3Edit(name, data, [&](const ImVec3& val) { script.instance->setFieldValue(name, Utils::FromImVec<glm::vec3>(val)); });
				break;
			}
			case ScriptFieldType::Vector4:
			{
				glm::vec4 data;
				script.instance->getFieldValue(name, data);
				Widgets::Vector4Edit(name, data, [&](const ImVec4& val) { script.instance->setFieldValue(name, Utils::FromImVec<glm::vec4>(val)); });
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
