#pragma once

#include "RenderLayer.h"

namespace Laby {

	class RenderStack
	{
	public:
		RenderStack() = default;
		virtual ~RenderStack();

		void pushLayer(RenderLayer* layer);
		void popLayer(RenderLayer* layer);
		void clearLayers();
		void clearItems();

		bool hasLayer(RenderLayer* layer);
		bool hasLayer(u8 layer);
		RenderLayer* getLayer(u8 layer);

		void addQuad(const TransformComponent& trComp, const SpriteRendererComponent& srComp, i32 entID = -1);
		void addCircle(const TransformComponent& trComp, const CircleRendererComponent& crComp, i32 entID = -1);

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