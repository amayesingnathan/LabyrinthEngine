#pragma once

#include <vector>

#include <Labyrinth/Assets/Asset.h>

#include "IRenderable.h"

namespace Laby {
	struct TransformComponent;
	struct SpriteRendererComponent;
	struct CircleRendererComponent;

	struct DrawData
	{
		enum DrawType { Quad, Circle };
		DrawType drawType;

		glm::mat4 transform;
		glm::vec4 colour;
		f32 tilingFactorOrThickness;

		// Not used for DrawType::Circle
		RenderType texType = RenderType::None;
		AssetHandle handle = 0;

		// Debug - Editor only
		i32 entityID;

		DrawData(const TransformComponent& trans, const SpriteRendererComponent& src, i32 entity = -1);
		DrawData(const TransformComponent& trans, const CircleRendererComponent& crc, i32 entity = -1);
	};

	class RenderLayer
	{
	public:
		RenderLayer(u8 layer) : mDepth(layer) {}
		virtual ~RenderLayer() = default;

		u8 GetDepth() const { return mDepth; }
		void Clear() { mElements.clear(); }

		void Draw() const;

		void AddQuad(const TransformComponent& trans, const SpriteRendererComponent& sprite, i32 entID)
		{
			mElements.emplace_back(trans, sprite, entID);
		}
		void AddCircle(const TransformComponent& trans, const CircleRendererComponent& circle, i32 entID)
		{
			mElements.emplace_back(trans, circle, entID);
		}

	private:
		u8 mDepth;
		std::vector<DrawData> mElements;

	};

}