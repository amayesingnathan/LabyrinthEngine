#include "Lpch.h"
#include "ScenePanel.h"

#include <Labyrinth/Editor/ModalManager.h>
#include <Labyrinth/Editor/SelectionManager.h>
#include <Labyrinth/Editor/Modals/NewBodyModal.h>
#include <Labyrinth/ImGui/ImGuiWidgets.h>

namespace Laby {

	ScenePanel::ScenePanel(const Ref<Scene>& scene)
		: mContext(scene)
	{
	}

	void ScenePanel::OnRender()
	{
		if (!mContext)
			return;

		Widgets::StringEdit("Name", mContext->GetName());
		Widgets::SameLine();
		Widgets::Button("Create..", []()
		{
			Widgets::OpenPopup("CreateEntityPopup");
		});

		Widgets::BeginPopup("CreateEntityPopup");
		Widgets::AddPopupItem("Empty Entity", [this]()
		{
			mSelectedEntity = mContext->CreateEntity("New Entity");
		});
		Widgets::AddPopupItem("Camera", [this]()
		{
			mSelectedEntity = mContext->CreateEntity("New Camera");
			mSelectedEntity.AddComponent<CameraComponent>();
		});
		Widgets::AddPopupItem("Rigid Body", [this]()
		{
			ModalManager::Open<NewBodyModal>("New Rigid Body...", ModalButtons::OKCancel, mContext);
		});
		Widgets::EndPopup();

		mContext->GetEntitiesWith<RootComponent>().each([&](auto entityID, auto& rc)
		{
			Entity entity{ entityID , mContext };
			DrawEntityNode(entity);
		});

		for (auto& entity : mToRemove)
			mContext->DestroyEntity(entity, false);
		mToRemove.clear();
	}

	void ScenePanel::OnSelectionChange()
	{
		const auto& selections = SelectionManager::GetSelections(SelectionDomain::Scene);
		mSelectedEntity = selections.size() != 0 ? mContext->FindEntity(selections[0]) : Entity{};

		if (mSelectedEntity && (!mPreviousEntity || mPreviousEntity != mSelectedEntity))
			mPreviousEntity = mSelectedEntity;
	}

	void ScenePanel::DrawEntityNode(Entity entity)
	{
		std::string& tag = entity.GetComponent<TagComponent>();
		auto& node = entity.GetComponent<NodeComponent>();

		Widgets::TreeNode((void*)&entity.GetComponent<IDComponent>().id, tag, mSelectedEntity == entity, [&]()
		{
			// Range for loop iterators may become invalidated if a new entity is added
			// to this node's children during looping, so ignore new additions for now
			// and they'll be drawn next render.
			size_t fixedSize = node.children.size();
			for (size_t i = 0; i < fixedSize; i++)
				DrawEntityNode(mContext->FindEntity(node.children[i]));
		});
		Widgets::OnWidgetSelected([=]() 
		{
			SelectionManager::DeselectAll(SelectionDomain::Scene);
			SelectionManager::Select(SelectionDomain::Scene, entity.GetUUID());
		});
		Widgets::AddDragDropSource("ENTITY_ITEM", entity.GetUUID());
		Widgets::AddDragDropTarget<UUID>("ENTITY_ITEM", [=](const UUID& id)
		{
			Entity dragEntity = mContext->FindEntity(id);
			if (dragEntity != entity)
				dragEntity.SetParent(entity);
		});

		Widgets::BeginContextPopup();
		Widgets::AddContextItem("Create Entity", [this]() 
		{
			mContext->CreateEntity("Empty Entity");
		});
		Widgets::AddContextItem("Create Child", [&]() 
		{
			mContext->CreateEntity("Empty Entity", entity);
		});
		Widgets::AddContextItem("Clone Entity", [&]() 
		{
			mContext->CloneEntity(entity);
		});
		Widgets::AddContextItem("Delete Entity", [&]() 
		{
			mToRemove.emplace_back(entity); 
			mSelectedEntity = {};
			mPreviousEntity = {};
		});
		Widgets::EndContextPopup();
	}
}