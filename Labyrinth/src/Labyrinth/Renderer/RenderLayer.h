#pragma once

#include <vector>

namespace Labyrinth {

	struct TransformComponent;
	struct SpriteRendererComponent;
	struct CircleRendererComponent;
	struct TilemapComponent;

	class QuadData
	{
	public:
		QuadData(const TransformComponent& trans, const SpriteRendererComponent& src, int id = -1) 
			: transform(&trans), sprite(&src), entID(id) {}

		const TransformComponent& getTrans() const { return *transform; }
		const SpriteRendererComponent& getSprite() const { return *sprite; }
		int getID() const { return entID; }

	private:
		const TransformComponent* transform;
		const SpriteRendererComponent* sprite;
		int entID;

	};

	class CircleData
	{
	public:
		CircleData(const TransformComponent& trans, const CircleRendererComponent& crc, int id = -1)
			: transform(&trans), circle(&crc), entID(id) {}

		const TransformComponent& getTrans() const { return *transform; }
		const CircleRendererComponent& getCircle() const { return *circle; }
		int getID() const { return entID; }

	private:
		const TransformComponent* transform;
		const CircleRendererComponent* circle;
		int entID;

	};

	class TilemapData
	{
	public:
		TilemapData(const TransformComponent& trans, const TilemapComponent& tmc)
			: transform(&trans), tilemap(&tmc) {}

		const TransformComponent& getTrans() const { return *transform; }
		const TilemapComponent& getMap() const { return *tilemap; }

	private:
		const TransformComponent* transform;
		const TilemapComponent* tilemap;

	};

	class LAB_API RenderLayer
	{
	public:
		RenderLayer(uint8_t layer) : mDepth(layer) {}
		virtual ~RenderLayer() = default;

		uint8_t getDepth() const { return mDepth; }
		void clear() { mQuads.clear(); mCircles.clear(); mTilemaps.clear(); }

		std::vector<QuadData>& getQuads() { return mQuads; }
		const std::vector<QuadData>& getQuads() const { return mQuads; }

		std::vector<CircleData>& getCircles() { return mCircles; }
		const std::vector<CircleData>& getCircles() const { return mCircles; }

		std::vector<TilemapData>& getTilemaps() { return mTilemaps; }
		const std::vector<TilemapData>& getTilemaps() const { return mTilemaps; }

		void addQuad(const TransformComponent& trans, const SpriteRendererComponent& sprite, int entID)
		{
			mQuads.emplace_back(trans, sprite, entID);
		}
		void addCircle(const TransformComponent& trans, const CircleRendererComponent& sprite, int entID)
		{
			mCircles.emplace_back(trans, sprite, entID);
		}
		void addTilemap(const TransformComponent& trans, const TilemapComponent& sprite)
		{
			mTilemaps.emplace_back(trans, sprite);
		}

	private:
		uint8_t mDepth;
		std::vector<QuadData> mQuads;
		std::vector<CircleData> mCircles;
		std::vector<TilemapData> mTilemaps;

	};

}