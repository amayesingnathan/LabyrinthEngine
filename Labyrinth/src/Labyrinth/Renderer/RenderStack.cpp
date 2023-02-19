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

	bool RenderStack::hasLayer(RenderLayer* layer)
	{
		return std::ranges::find(mLayers, layer) != mLayers.end();
	}

	bool RenderStack::hasLayer(u8 layer)
	{
		return std::ranges::find_if(mLayers, [&](const RenderLayer* rLayer)
		{
			return rLayer->getDepth() == layer;
		}) != mLayers.end();
	}

	RenderLayer* RenderStack::getLayer(u8 layer)
	{
		auto it = std::ranges::find_if(mLayers, [&](const RenderLayer* rLayer)
		{
			return rLayer->getDepth() == layer;
		});
		
		if (it == mLayers.end())
			return nullptr;

		return *it;
	}

	void RenderStack::pushLayer(RenderLayer* layer)
	{
		if (!hasLayer(layer))
			mLayers.emplace_back(layer);
	}

	void RenderStack::popLayer(RenderLayer* layer)
	{
		auto it = std::find(mLayers.begin(), mLayers.end(), layer);
		if (it != mLayers.end())
			mLayers.erase(it);
	}

	void RenderStack::clearLayers()
	{
		for (RenderLayer* layer : mLayers)
			delete layer;

		mLayers.clear();
	}

	void RenderStack::clearItems()
	{
		for (RenderLayer* layer : mLayers)
			layer->clear();
	}

	void RenderStack::addQuad(const TransformComponent& trComp, const SpriteRendererComponent& srComp, i32 entID)
	{
		RenderLayer* targetLayer = getLayer(srComp.layer);

		if (!targetLayer)
		{
			targetLayer = new RenderLayer(srComp.layer);
			pushLayer(targetLayer);
		}

		targetLayer->addQuad(trComp, srComp, entID);
	}

	void RenderStack::addCircle(const TransformComponent& trComp, const CircleRendererComponent& crComp, i32 entID)
	{
		RenderLayer* targetLayer = getLayer(crComp.layer);

		if (!targetLayer)
		{
			targetLayer = new RenderLayer(crComp.layer);
			pushLayer(targetLayer);
		}

		targetLayer->addCircle(trComp, crComp, entID);
	}

	void RenderStack::draw()
	{
		std::sort(mLayers.begin(), mLayers.end(), [](const auto& lhs, const auto& rhs)
		{
			return lhs->getDepth() < rhs->getDepth();
		});

		for (RenderLayer* layer : mLayers)
			layer->draw();
	}
}