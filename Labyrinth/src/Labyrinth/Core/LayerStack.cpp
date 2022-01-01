#include "Lpch.h"
#include "LayerStack.h"

namespace Labyrinth {

	LayerStack::LayerStack()
	{
		mLayerInsert = mLayers.begin();
	}

	LayerStack::~LayerStack()
	{
		for (Layer* layer : mLayers)
		{
			delete layer;
		}
	}

	void LayerStack::pushLayer(Layer* layer)
	{
		mLayerInsert = mLayers.emplace(mLayerInsert, layer);
	}

	void LayerStack::pushOverlay(Layer* overlay)
	{
		mLayers.emplace_back(overlay);
	}

	void LayerStack::popLayer(Layer* layer)
	{
		auto it = std::find(mLayers.begin(), mLayers.end(), layer);
		if (it != mLayers.end())
		{
			mLayers.erase(it);
			mLayerInsert--;
		}
	}

	void LayerStack::popOverlay(Layer* layer)
	{
		auto it = std::find(mLayers.begin(), mLayers.end(), layer);
		if (it != mLayers.end())
		{
			mLayers.erase(it);
		}
	}
}
