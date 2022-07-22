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

	bool RenderStack::hasLayer(u8 layer)
	{
		return std::find_if(mLayers.begin(), mLayers.end(), [&](const RenderLayer* rLayer) 
			{
				return rLayer->getDepth() == layer;
			}) != mLayers.end();
	}

	RenderLayer* RenderStack::getLayer(u8 layer)
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

	void RenderStack::addTilemap(const TransformComponent& trComp, const TilemapComponent& tmComp)
	{
		RenderLayer* targetLayer = getLayer(tmComp.layer);

		if (!targetLayer)
		{
			targetLayer = new RenderLayer(tmComp.layer);
			pushLayer(targetLayer);
		}

		targetLayer->addTilemap(trComp, tmComp);
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

			for (const CircleData& circle : layer->getCircles())
				Renderer2D::DrawCircle(circle.getTrans(), circle.getCircle(), circle.getID());

			for (const TilemapData& tilemap : layer->getTilemaps())
				Renderer2D::DrawQuad(tilemap.getTrans(), tilemap.getMap().getTex());
		}
	}
}