#pragma once

#include <Labyrinth/Core/System/Base.h>

struct b2Vec2;

namespace Laby {

	struct MapShape
	{
		glm::vec2* vertices = nullptr;
		i32 vertexCount = 0;

		MapShape(i32 numVertices) : vertices(new glm::vec2[numVertices]), vertexCount(numVertices) {}
		void free() { delete[] vertices; }
		const b2Vec2* get() const { return StaticCast<b2Vec2>(vertices); }
	};


}