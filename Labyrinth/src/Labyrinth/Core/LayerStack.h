#pragma once

#include "Labyrinth/Core/System/Base.h"
#include "Layer.h"

#include "Labyrinth/Networking/NetworkLayer.h"

namespace Labyrinth {

	class LayerStack
	{
	public:
		LayerStack() = default;
		~LayerStack();

		void pushLayer(Layer* layer);
		void pushLayer(NetworkLayer* layer);
		void pushOverlay(Layer* overlay);
		void popLayer(Layer* layer);
		void popOverlay(Layer* overlay);

		std::vector<Layer*>::iterator begin() { return mLayers.begin(); }
		std::vector<Layer*>::iterator end() { return mLayers.end(); }
		std::vector<Layer*>::reverse_iterator rbegin() { return mLayers.rbegin(); }
		std::vector<Layer*>::reverse_iterator rend() { return mLayers.rend(); }

		std::vector<Layer*>::const_iterator begin() const { return mLayers.begin(); }
		std::vector<Layer*>::const_iterator end()	const { return mLayers.end(); }
		std::vector<Layer*>::const_reverse_iterator rbegin() const { return mLayers.rbegin(); }
		std::vector<Layer*>::const_reverse_iterator rend() const { return mLayers.rend(); }

	private:
		std::vector<Layer*> mLayers;
		std::vector<NetworkLayer*> mNetLayers;
		unsigned int mLayerInsertIndex = 0;
	};

}

