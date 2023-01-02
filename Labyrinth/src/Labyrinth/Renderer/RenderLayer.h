#pragma once

#include <vector>

#include <Labyrinth/Core/System/Base.h>

#include "Renderer2D.h"

namespace Laby {

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
			: mTransform(&trans), mCircle(&crc), mEntID(id) {}

		const TransformComponent& getTrans() const { return *mTransform; }
		const CircleRendererComponent& getCircle() const { return *mCircle; }
		i32 getID() const { return mEntID; }

	private:
		const TransformComponent* mTransform;
		const CircleRendererComponent* mCircle;
		i32 mEntID;

	};

	using RenderElements = TypeList<QuadData, CircleData>;

	class RenderData
	{
	public:
		RenderData() = delete;
		RenderData(const TransformComponent& trans, const SpriteRendererComponent& sprite, i32 entID)
			: mElement(QuadData(trans, sprite, entID)) {}
		RenderData(const TransformComponent& trans, const CircleRendererComponent& circle, i32 entID)
			: mElement(CircleData(trans, circle, entID)) {}

		void draw() const
		{
			std::visit([](auto&& arg)
			{
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, QuadData>)
					Renderer2D::DrawSprite(arg.getTrans(), arg.getSprite(), arg.getID());
				else if (std::is_same_v<T, CircleData>)
					Renderer2D::DrawCircle(arg.getTrans(), arg.getCircle(), arg.getID());
				else
					LAB_STATIC_ASSERT(false, "non-exhaustive visitor!");
			}, mElement);
		}

	private:
		RenderElements::VariantType mElement;
	};

	class RenderLayer
	{
	public:
		RenderLayer(u8 layer) : mDepth(layer) {}
		virtual ~RenderLayer() = default;

		u8 getDepth() const { return mDepth; }
		void clear() { mElements.clear(); }

		void draw() const
		{
			for (const RenderData& element : mElements)
				element.draw();
		}

		void addQuad(const TransformComponent& trans, const SpriteRendererComponent& sprite, i32 entID)
		{
			mElements.emplace_back(trans, sprite, entID);
		}
		void addCircle(const TransformComponent& trans, const CircleRendererComponent& circle, i32 entID)
		{
			mElements.emplace_back(trans, circle, entID);
		}

	private:
		u8 mDepth;
		std::vector<RenderData> mElements;

	};

}