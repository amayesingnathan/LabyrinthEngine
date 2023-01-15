#include "Lpch.h"
#include "EntityPanel.h"

#include <Labyrinth/Editor/EditorResources.h>
#include <Labyrinth/Editor/SelectionManager.h>
#include <Labyrinth/ImGui/ImGuiWidgets.h>
#include <Labyrinth/Renderer/Renderer.h>
#include <Labyrinth/Tools/EnumUtils.h>

namespace Laby {

	using ParentEntityEntry = ComboEntry<Entity>;

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

	EntityPanel::EntityPanel()
	{
		FramebufferSpec fbSpec;
		fbSpec.width = 200;
		fbSpec.height = 200;
		fbSpec.attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.samples = 1;

		mTexture = Ref<Framebuffer>::Create(fbSpec);
	}

	void EntityPanel::onUpdate(Timestep ts)
	{
		if (!mSelectedEntity)
			return;

		if (!mSelectedEntity.hasComponent<SpriteRendererComponent>())
			return;

		auto& component = mSelectedEntity.getComponent<SpriteRendererComponent>();

		// Draw texture/subtexture to framebuffer
		mTexture->bind();

		Renderer::SetClearColor({ 0.0f, 0.0f, 0.0f, 0.0f });
		Renderer::Clear();

		Renderer2D::BeginState();

		std::string texLabel;
		switch (component.type)
		{
		case SpriteRendererComponent::TexType::None:
		{
			Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 1.0f, 1.0f }, EditorResources::NoTexture);
			break;
		}

		case SpriteRendererComponent::TexType::Texture:
		{
			Ref<Texture2D> tex = component.handle ? AssetManager::GetAsset<Texture2D>(component.handle) : EditorResources::NoTexture;
			Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 1.0f, 1.0f }, tex);
			break;
		}

		case SpriteRendererComponent::TexType::SubTexture:
		{
			Ref<SubTexture2D> subtex = component.handle ? AssetManager::GetAsset<SubTexture2D>(component.handle) : EditorResources::NoSubTexture;
			Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 1.0f, 1.0f }, subtex);
			break;
		}
		}

		Renderer2D::EndState();
		mTexture->unbind();
	}

	void EntityPanel::onImGuiRender()
	{
		if (!mSelectedEntity)
			return;

		DrawComponents();
	}

	void EntityPanel::onSelectionChange()
	{
		if (mSelectedEntity && mPreviousEntity != mSelectedEntity)
			mPreviousEntity = mSelectedEntity;

		const auto& selections = SelectionManager::GetSelections(SelectionDomain::Scene);
		mSelectedEntity = selections.size() != 0 ? mContext->findEntity(selections[0]) : Entity{};
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
		ImGuiUtils::PushItemWidth(-1);

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

		ImGuiUtils::PopItemWidth();

		if (mSelectedEntity.hasComponent<NodeComponent>())
		{
			std::vector<ParentEntityEntry> comboEntries;
			comboEntries.emplace_back("None", Entity{});

			// Create map of possible parents
			mContext->mRegistry.view<TagComponent, IDComponent>().each([&](auto entityID, auto& tc, auto& idc)
			{
				if (mSelectedEntity != entityID)
					comboEntries.emplace_back(fmt::format("{}\rID = ({})", tc.tag, idc.id.to_string()), Entity{entityID, mContext});
			});

			std::sort(comboEntries.begin(), comboEntries.end(), [](const ParentEntityEntry& u, const ParentEntityEntry& v) { return u.value.getUUID() < v.value.getUUID(); });

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
				[&](SceneCamera::ProjectionType projType) { camera.setProjectionType(projType); });

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

			Ref<IRenderable> tex = mTexture ? mTexture.to<IRenderable>() : EditorResources::NoTexture.to<IRenderable>();
			Widgets::Label("Texture");
			Widgets::Image(tex, glm::vec2{ ImGuiUtils::AvailableRegion().x - 15.0f, 100.0f });
		});
	}

	void EntityPanel::DrawChildControllerElement(const std::string& name, glm::vec3& componentElement, glm::vec3& displayElement, glm::vec3& lastDisplay, float min, float max, ImGuiSliderFlags flags)
	{
	}

	void EntityPanel::DrawScriptClassFields(Entity entity)
	{
	}

	void EntityPanel::DrawScriptInstanceFields(Entity entity)
	{
	}
}
