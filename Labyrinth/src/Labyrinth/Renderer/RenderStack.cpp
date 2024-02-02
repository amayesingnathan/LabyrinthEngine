#include "Lpch.h"
#include "RenderStack.h"

#include "Renderer2D.h"
#include <Labyrinth/Scene/Components.h>

namespace Laby {

	RenderStack::~RenderStack()
	{
		for (RenderLayer* layer : mLayers)
			delete layer;
	}

	bool RenderStack::HasLayer(RenderLayer* layer)
	{
		return std::ranges::find(mLayers, layer) != mLayers.end();
	}

	bool RenderStack::HasLayer(u8 layer)
	{
		return std::ranges::find_if(mLayers, [&](const RenderLayer* rLayer)
		{
			return rLayer->GetDepth() == layer;
		}) != mLayers.end();
	}

	RenderLayer* RenderStack::GetLayer(u8 layer)
	{
		auto it = std::ranges::find_if(mLayers, [&](const RenderLayer* rLayer)
		{
			return rLayer->GetDepth() == layer;
		});
		
		if (it == mLayers.end())
			return nullptr;

		return *it;
	}

	void RenderStack::PushLayer(RenderLayer* layer)
	{
		if (!HasLayer(layer))
			mLayers.emplace_back(layer);
	}

	void RenderStack::PopLayer(RenderLayer* layer)
	{
		auto it = std::find(mLayers.begin(), mLayers.end(), layer);
		if (it != mLayers.end())
			mLayers.erase(it);
	}

	void RenderStack::ClearLayers()
	{
		for (RenderLayer* layer : mLayers)
			delete layer;

		mLayers.clear();
	}

	void RenderStack::ClearItems()
	{
		for (RenderLayer* layer : mLayers)
			layer->Clear();
	}

	void RenderStack::AddQuad(const TransformComponent& trComp, const SpriteRendererComponent& srComp, i32 entID)
	{
		RenderLayer* targetLayer = GetLayer(srComp.layer);

		if (!targetLayer)
		{
			targetLayer = new RenderLayer(srComp.layer);
			PushLayer(targetLayer);
		}

		targetLayer->AddQuad(trComp, srComp, entID);
	}

	void RenderStack::AddCircle(const TransformComponent& trComp, const CircleRendererComponent& crComp, i32 entID)
	{
		RenderLayer* targetLayer = GetLayer(crComp.layer);

		if (!targetLayer)
		{
			targetLayer = new RenderLayer(crComp.layer);
			PushLayer(targetLayer);
		}

		targetLayer->AddCircle(trComp, crComp, entID);
	}

	void RenderStack::Draw()
	{
		std::sort(mLayers.begin(), mLayers.end(), [](const auto& lhs, const auto& rhs)
		{
			return lhs->GetDepth() < rhs->GetDepth();
		});

		for (RenderLayer* layer : mLayers)
			layer->Draw();
	}
}