#pragma once

#include <vector>

namespace Labyrinth {

	struct TransformComponent;
	struct SpriteRendererComponent;
	struct CircleRendererComponent;

	class QuadData
	{
	public:
		QuadData(const TransformComponent& trans, const SpriteRendererComponent& src, i32 id = -1) 
			: transform(&trans), sprite(&src), entID(id) {}

		const TransformComponent& getTrans() const { return *transform; }
		const SpriteRendererComponent& getSprite() const { return *sprite; }
		i32 getID() const { return entID; }

	private:
		const TransformComponent* transform;
		const SpriteRendererComponent* sprite;
		i32 entID;

	};

	class CircleData
	{
	public:
		CircleData(const TransformComponent& trans, const CircleRendererComponent& crc, i32 id = -1)
			: transform(&trans), circle(&crc), entID(id) {}

		const TransformComponent& getTrans() const { return *transform; }
		const CircleRendererComponent& getCircle() const { return *circle; }
		i32 getID() const { return entID; }

	private:
		const TransformComponent* transform;
		const CircleRendererComponent* circle;
		i32 entID;

	};

	class LAB_API RenderLayer
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

		void addQuad(const TransformComponent& trans, const SpriteRendererComponent& sprite, i32 entID)
		{
			mQuads.emplace_back(trans, sprite, entID);
		}
		void addCircle(const TransformComponent& trans, const CircleRendererComponent& sprite, i32 entID)
		{
			mCircles.emplace_back(trans, sprite, entID);
		}

	private:
		u8 mDepth;
		std::vector<QuadData> mQuads;
		std::vector<CircleData> mCircles;

	};

}