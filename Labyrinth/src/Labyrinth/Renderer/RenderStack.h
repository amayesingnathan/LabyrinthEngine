#pragma once

#include "RenderLayer.h"

namespace Labyrinth {

	class LAB_API RenderStack
	{
	public:
		RenderStack() = default;
		virtual ~RenderStack();

		void pushLayer(RenderLayer* layer);
		void popLayer(RenderLayer* layer);
		void clear();
		void clearQuads();

		bool hasLayer(RenderLayer* layer);
		bool hasLayer(uint8_t layer);
		RenderLayer* getLayer(uint8_t layer);

		void addQuad(const TransformComponent& trComp, const SpriteRendererComponent& srComp, int entID = -1);

		void draw();


		std::vector<RenderLayer*>::iterator begin() { return mLayers.begin(); }
		std::vector<RenderLayer*>::iterator end() { return mLayers.end(); }
		std::vector<RenderLayer*>::reverse_iterator rbegin() { return mLayers.rbegin(); }
		std::vector<RenderLayer*>::reverse_iterator rend() { return mLayers.rend(); }

		std::vector<RenderLayer*>::const_iterator begin() const { return mLayers.begin(); }
		std::vector<RenderLayer*>::const_iterator end()	const { return mLayers.end(); }
		std::vector<RenderLayer*>::const_reverse_iterator rbegin() const { return mLayers.rbegin(); }
		std::vector<RenderLayer*>::const_reverse_iterator rend() const { return mLayers.rend(); }

	private:
		std::vector<RenderLayer*> mLayers;

	};

}