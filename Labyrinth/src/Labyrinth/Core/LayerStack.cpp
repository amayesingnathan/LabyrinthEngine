#include "Lpch.h"
#include "LayerStack.h"

namespace Labyrinth {

	LayerStack::~LayerStack()
	{
		for (Layer* layer : mLayers)
		{
			layer->onDetach();
			delete layer;
		}
	}

	void LayerStack::pushLayer(Layer* layer)
	{
		mLayers.emplace(mLayers.begin() + mLayerInsertIndex, layer);
		mLayerInsertIndex++;
		layer->onAttach();
	}

	void LayerStack::pushOverlay(Layer* overlay)
	{
		mLayers.emplace_back(overlay);	
		overlay->onAttach();
	}

	void LayerStack::popLayer(Layer* layer)
	{
		auto it = std::find(mLayers.begin(), mLayers.begin() + mLayerInsertIndex, layer);
		if (it != mLayers.begin() + mLayerInsertIndex)
		{
			layer->onDetach();
			mLayers.erase(it);
			mLayerInsertIndex--;
		}
	}

	void LayerStack::popOverlay(Layer* overlay)
	{
		auto it = std::find(mLayers.begin() + mLayerInsertIndex, mLayers.end(), overlay);
		if (it != mLayers.end())
		{
			overlay->onDetach();
			mLayers.erase(it);
		}
	}
}
