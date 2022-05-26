#include "Lpch.h"
#include "RenderStack.h"

#include "Renderer2D.h"

namespace Labyrinth {

	RenderStack::~RenderStack()
	{
		for (RenderLayer* layer : mLayers)
			delete layer;
	}

	bool RenderStack::hasLayer(RenderLayer* layer)
	{
		return std::find(mLayers.begin(), mLayers.end(), layer) != mLayers.end();
	}

	bool RenderStack::hasLayer(uint8_t layer)
	{
		return std::find_if(mLayers.begin(), mLayers.end(), [&](const RenderLayer* rLayer) 
			{
				return rLayer->getDepth() == layer;
			}) != mLayers.end();
	}

	RenderLayer* RenderStack::getLayer(uint8_t layer)
	{
		auto it = std::find_if(mLayers.begin(), mLayers.end(), [&](const RenderLayer* rLayer)
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

	void RenderStack::clear()
	{
		for (RenderLayer* layer : mLayers)
			delete layer;

		mLayers.clear();
	}

	void RenderStack::addQuad(const TransformComponent& trComp, const SpriteRendererComponent& srComp, int entID)
	{
		RenderLayer* targetLayer = getLayer(srComp.layer);

		if (!targetLayer)
		{
			targetLayer = new RenderLayer(srComp.layer);
			pushLayer(targetLayer);
		}

		targetLayer->addQuad(trComp, srComp, entID);
	}

	void RenderStack::draw()
	{
		std::sort(mLayers.begin(), mLayers.end(), [](const auto& lhs, const auto& rhs)
			{
				return lhs->getDepth() < rhs->getDepth();
			});

		for (RenderLayer* layer : mLayers)
		{
			for (const QuadData& quad : layer->getQuads())
				Renderer2D::DrawSprite(quad.getTrans(), quad.getSprite(), quad.getID());
		}
	}
}