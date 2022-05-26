#pragma once

#include <vector>

namespace Labyrinth {

	struct TransformComponent;
	struct SpriteRendererComponent;

	class QuadData
	{
	public:
		QuadData(const TransformComponent& trans, const SpriteRendererComponent& sr, int id = -1) : transform(&trans), sprite(&sr), entID(id) {}

		const TransformComponent& getTrans() const { return *transform; }
		const SpriteRendererComponent& getSprite() const { return *sprite; }
		int getID() const { return entID; }

	private:
		const TransformComponent* transform;
		const SpriteRendererComponent* sprite;
		int entID;

	};

	class LAB_API RenderLayer
	{
	public:
		RenderLayer(uint8_t layer) : mDepth(layer) {}
		virtual ~RenderLayer() = default;

		uint8_t getDepth() const { return mDepth; }

		std::vector<QuadData>& getQuads() { return mQuads; }
		const std::vector<QuadData>& getQuads() const { return mQuads; }

		template<typename... Args>
		void addQuad(Args... args)
		{
			mQuads.emplace_back(std::forward<Args>(args)...);
		}

	private:
		uint8_t mDepth;
		std::vector<QuadData> mQuads;

	};

}