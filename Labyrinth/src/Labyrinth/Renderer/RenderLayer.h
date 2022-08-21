#pragma once

#include <vector>

namespace Labyrinth {

	struct TransformComponent;
	struct SpriteRendererComponent;
	struct CircleRendererComponent;
	struct TilemapControllerComponent;

	class QuadData
	{
	public:
		QuadData(const TransformComponent& trans, const SpriteRendererComponent& src, i32 id = -1) 
			: mTransform(&trans), mSprite(&src), mEntID(id) {}

		const TransformComponent& getTrans() const { return *mTransform; }
		const SpriteRendererComponent& getSprite() const { return *mSprite; }
		i32 getID() const { return mEntID; }

	private:
		const TransformComponent* mTransform;
		const SpriteRendererComponent* mSprite;
		i32 mEntID;

	};

	class CircleData
	{
	public:
		CircleData(const TransformComponent& trans, const CircleRendererComponent& crc, i32 id = -1)
			: mTransform(&trans), mCircle(&crc), mEntID(id) {}

		const TransformComponent& getTrans() const { return *mTransform; }
		const CircleRendererComponent& getCircle() const { return *mCircle; }
		i32 getID() const { return mEntID; }

	private:
		const TransformComponent* mTransform;
		const CircleRendererComponent* mCircle;
		i32 mEntID;

	};

	class TilemapData
	{
	public:
		TilemapData(const TransformComponent& trans, const TilemapControllerComponent& tmc, i32 id = -1)
			: mTransform(&trans), mTilemap(&tmc), mEntID(id) {}

		const TransformComponent& getTrans() const { return *mTransform; }
		const TilemapControllerComponent& getMap() const { return *mTilemap; }
		i32 getID() const { return mEntID; }

	private:
		const TransformComponent* mTransform;
		const TilemapControllerComponent* mTilemap;
		i32 mEntID;

	};

	class RenderLayer
	{
	public:
		RenderLayer(u8 layer) : mDepth(layer) {}
		virtual ~RenderLayer() = default;

		u8 getDepth() const { return mDepth; }
		void clear() { mQuads.clear(); mCircles.clear(); }

		std::vector<QuadData>& getQuads() { return mQuads; }
		const std::vector<QuadData>& getQuads() const { return mQuads; }

		std::vector<CircleData>& getCircles() { return mCircles; }
		const std::vector<CircleData>& getCircles() const { return mCircles; }

		std::vector<TilemapData>& getTilemaps() { return mTilemaps; }
		const std::vector<TilemapData>& getTilemaps() const { return mTilemaps; }

		void addQuad(const TransformComponent& trans, const SpriteRendererComponent& sprite, i32 entID)
		{
			mQuads.emplace_back(trans, sprite, entID);
		}
		void addCircle(const TransformComponent& trans, const CircleRendererComponent& sprite, i32 entID)
		{
			mCircles.emplace_back(trans, sprite, entID);
		}
		void addCircle(const TransformComponent& trans, const TilemapControllerComponent& tilemap, i32 entID)
		{
			mTilemaps.emplace_back(trans, tilemap, entID);
		}

	private:
		u8 mDepth;
		std::vector<QuadData> mQuads;
		std::vector<CircleData> mCircles;
		std::vector<TilemapData> mTilemaps;

	};

}