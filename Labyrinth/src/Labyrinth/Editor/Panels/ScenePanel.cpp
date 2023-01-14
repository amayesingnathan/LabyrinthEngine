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

	void ScenePanel::onImGuiRender()
	{
		Widgets::StringEdit("Name", mContext->mName);
		Widgets::SameLine();
		Widgets::Button("Create..", []()
		{
			Widgets::OpenPopup("CreateEntityPopup");
		});

		UI::PopUp* createEntityPopup = Widgets::BeginPopup("CreateEntityPopup");
		Widgets::AddMenuItem(createEntityPopup, "Empty Entity", [this]()
		{
			mSelectedEntity = mContext->CreateEntity("New Entity");
		});
		Widgets::AddMenuItem(createEntityPopup, "Camera", [this]()
		{
			mSelectedEntity = mContext->CreateEntity("New Camera");
			mSelectedEntity.addComponent<CameraComponent>();
		});
		Widgets::AddMenuItem(createEntityPopup, "Rigid Body", [this]()
		{
			ModalManager::Open<NewBodyModal>("New Rigid Body...", ModalButtons::OKCancel, mContext);
		});
		Widgets::EndPopup(createEntityPopup);

		mContext->mRegistry.view<RootComponent>().each([&](auto entityID, auto& rc)
		{
			Entity entity{ entityID , mContext };
			DrawEntityNode(entity);
		});

		for (auto& entity : mToRemove)
			mContext->DestroyEntity(entity, false);
		mToRemove.clear();
	}

	void ScenePanel::onSelectionChange()
	{
		if (mSelectedEntity && mPreviousEntity != mSelectedEntity)
			mPreviousEntity = mSelectedEntity;

		const auto& selections = SelectionManager::GetSelections(SelectionDomain::Scene);
		mSelectedEntity = mContext->findEntity(selections.size() != 0 ? selections[0] : UUID(0));
	}

	void ScenePanel::DrawEntityNode(Entity entity)
	{
		std::string& tag = entity.getComponent<TagComponent>();
		auto& node = entity.getComponent<NodeComponent>();

		Widgets::TreeNode(entity.getUUID(), tag, mSelectedEntity == entity, [&]()
		{
			// Range for loop iterators may become invalidated if a new entity is added
			// to this node's children during looping, so ignore new additions for now
			// and they'll be drawn next render.
			size_t fixedSize = node.children.size();
			for (size_t i = 0; i < fixedSize; i++)
				DrawEntityNode(mContext->findEntity(node.children[i]));
		});
		Widgets::OnWidgetSelected([=]() 
		{
			SelectionManager::DeselectAll(SelectionDomain::Scene);
			SelectionManager::Select(SelectionDomain::Scene, entity.getUUID());
		});
		Widgets::AddDragDropSource("ENTITY_ITEM", entity.getUUID());
		Widgets::AddDragDropTarget<UUID>("ENTITY_ITEM", [=](const UUID& id)
		{
			Entity dragEntity = mContext->findEntity(id);
			if (dragEntity != entity)
				dragEntity.setParent(entity);
		});

		UI::PopUpContext* popup = Widgets::BeginContextPopup();
		Widgets::AddMenuItem(popup, "Create Entity", [this]() 
		{
			mContext->CreateEntity("Empty Entity");
		});
		Widgets::AddMenuItem(popup, "Create Child", [&]() 
		{
			mContext->CreateEntity("Empty Entity", entity);
		});
		Widgets::AddMenuItem(popup, "Clone Entity", [&]() 
		{
			mContext->CloneEntity(entity);
		});
		Widgets::AddMenuItem(popup, "Delete Entity", [&]() 
		{
			mToRemove.emplace_back(entity); // Queue up to delete after range for loop so it isn't invalidated.
			mSelectedEntity = {};
			mPreviousEntity = {};
		});
		Widgets::EndContextPopup(popup);
	}
}