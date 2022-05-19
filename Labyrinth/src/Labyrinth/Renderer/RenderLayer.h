#pragma once

namespace Labyrinth {

	struct TransformComponent;
	struct SpriteRendererComponent;

	struct QuadData
	{
		const TransformComponent& transform;
		const SpriteRendererComponent& sprite;
		int entID;

		QuadData(const TransformComponent& trans, const SpriteRendererComponent& sr, int id = -1) : transform(trans), sprite(sr), entID(id) {}
	};

	class RenderLayer
	{
	public:
		RenderLayer(uint8_t layer) : mDepth(layer) {}
		virtual ~RenderLayer() = default;

		uint8_t getDepth() const { return mDepth; }

		std::vector<QuadData>& getQuads() { return mQuads; }
		const std::vector<QuadData>& getQuads() const { return mQuads; }

		void addQuad(const QuadData& quad)
		{
			mQuads.emplace_back(quad);
		}

	private:
		uint8_t mDepth;
		std::vector<QuadData> mQuads;

	};

}