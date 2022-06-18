#pragma once

#include <vector>

namespace Labyrinth {

	struct TransformComponent;
	struct SpriteRendererComponent;
	struct CircleRendererComponent;

	class QuadData
	{
	public:
		QuadData(const TransformComponent& trans, const SpriteRendererComponent& sr, int id = -1) 
			: transform(&trans), sprite(&sr), entID(id) {}

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
		CircleData(const TransformComponent& trans, const CircleRendererComponent& cr, int id = -1)
			: transform(&trans), circle(&cr), entID(id) {}

		const TransformComponent& getTrans() const { return *transform; }
		const CircleRendererComponent& getCircle() const { return *circle; }
		int getID() const { return entID; }

	private:
		const TransformComponent* transform;
		const CircleRendererComponent* circle;
		int entID;

	};

	class LAB_API RenderLayer
	{
	public:
		RenderLayer(uint8_t layer) : mDepth(layer) {}
		virtual ~RenderLayer() = default;

		uint8_t getDepth() const { return mDepth; }
		void clear() { mQuads.clear(); mCircles.clear(); }

		std::vector<QuadData>& getQuads() { return mQuads; }
		const std::vector<QuadData>& getQuads() const { return mQuads; }

		std::vector<CircleData>& getCircles() { return mCircles; }
		const std::vector<CircleData>& getCircles() const { return mCircles; }

		void addQuad(const TransformComponent& trans, const SpriteRendererComponent& sprite, int entID)
		{
			mQuads.emplace_back(trans, sprite, entID);
		}
		void addCircle(const TransformComponent& trans, const CircleRendererComponent& sprite, int entID)
		{
			mCircles.emplace_back(trans, sprite, entID);
		}

	private:
		uint8_t mDepth;
		std::vector<QuadData> mQuads;
		std::vector<CircleData> mCircles;

	};

}