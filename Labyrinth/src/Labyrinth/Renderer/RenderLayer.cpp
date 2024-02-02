#include "Lpch.h"
#include "RenderLayer.h"

#include <Labyrinth/Scene/Components.h>

#include "Renderer2D.h"

namespace Laby {

	DrawData::DrawData(const TransformComponent& trans, const SpriteRendererComponent& src, i32 entity)
		: drawType(Quad), transform(trans), colour(src.colour), tilingFactorOrThickness(src.tilingFactor),
		  texType(src.type), handle(src.handle),
		  entityID(entity) {}

	DrawData::DrawData(const TransformComponent& trans, const CircleRendererComponent& crc, i32 entity)
		: drawType(Circle), transform(trans), colour(crc.colour), tilingFactorOrThickness(crc.thickness),
		  entityID(entity) {}

	void RenderLayer::Draw() const
	{
		for (const DrawData& element : mElements)
			Renderer2D::DrawElement(element);
	}
}