#include "Lpch.h"
#include "TileBehaviourLayer.h"

namespace Laby {

    ChainShape::ChainShape(const Shape& shape)
        : vertices(new glm::vec2[shape.size()]), vertexCount(shape.size())
    {
        for (usize i = 0; i < vertexCount; i++)
        {
            vertices[i].x = (f32)shape[i].x;
            vertices[i].y = (f32)shape[i].y;
        }
    }

    std::vector<ChainShape> TileBehaviourLayer::getShapes() const
    {
        std::vector<ChainShape> mapShapes;
        std::vector<Shape>&& contiguousShapes = GetContiguousShapes();

        for (const Shape& shape : contiguousShapes)
            mapShapes.emplace_back(std::move(TraverseShape(shape)));

        return mapShapes;
    }

    using Direction = std::pair<int, int>;
    static constexpr Direction sDirections[4] = { { 1, 0 }, { 0, 1 }, { -1, 0 }, { 0, -1 } };

    static GridPos AddDirection(const GridPos& pos, int index) { return { pos.x + sDirections[index].first, pos.y + sDirections[index].second }; }
    static GridPos GetLowestNeighbour(const std::map<GridPos, usize>& vertexCount, const GridPos& currentPos, usize& currentDirectionIndex);
    static void TraverseR(const std::map<GridPos, usize>& vertexCount, Shape& vertices, GridPos& currentPos, usize& dIndex);

    ChainShape TileBehaviourLayer::TraverseShape(const Shape& shape) const
    {
        // Add all vertices of all squares 
        std::map<GridPos, usize> vertexCount;
        for (const GridPos& pos : shape)
        {
            vertexCount[pos]++;
            vertexCount[{ pos.x + 1, pos.y }]++;
            vertexCount[{ pos.x, pos.y + 1 }]++;
            vertexCount[{ pos.x + 1, pos.y + 1 }]++;
        }

        // Should be top left vertex
        const auto& [pos, count] = *vertexCount.begin();
        GridPos nextPos = { pos.x + 1, pos.y };
        LAB_CORE_ASSERT(vertexCount.contains(nextPos) && vertexCount.at(nextPos) == 2, "Not at correct vertex!");

        GridPos currentPos = pos;
        usize dIndex = 3;
        Shape vertices;
        TraverseR(vertexCount, vertices, currentPos, dIndex);

        return vertices;
    }

    std::vector<Shape> TileBehaviourLayer::GetContiguousShapes() const
    {
        Grid<bool> checkedGrid(mWidth, mHeight);

        std::vector<Shape> shapes;
        for (usize y = 0; y < mHeight; y++)
        {
            for (usize x = 0; x < mWidth; x++)
            {
                if (!checkedGrid(x, y) && At(x, y).solid)
                {
                    Shape&& shape = FloodFill(x, y, checkedGrid);
                    shapes.emplace_back(std::move(shape));
                }
            }
        }
        return shapes;
    }

    Shape TileBehaviourLayer::FloodFill(usize x, usize y, Grid<bool>& checkedGrid) const
    {
        Shape shape;
        if (x == Limits::usizeMax || x >= mWidth || y == Limits::usizeMax || y >= mHeight)
            return shape;

        if (checkedGrid(x, y))
            return;

        if (!At(x, y).solid)
            return shape;

        shape.emplace_back(x, y);
        checkedGrid(x, y) = true;

        Shape&& neighbours = FloodFill(x - 1, y, checkedGrid);
        shape.emplace(shape.end(), neighbours.begin(), neighbours.end());

        neighbours = FloodFill(x + 1, y, checkedGrid);
        shape.emplace(shape.end(), neighbours.begin(), neighbours.end());

        neighbours = FloodFill(x, y - 1, checkedGrid);
        shape.emplace(shape.end(), neighbours.begin(), neighbours.end());

        neighbours = FloodFill(x, y + 1, checkedGrid);
        shape.emplace(shape.end(), neighbours.begin(), neighbours.end());

        return shape;
    }



    /*
        Static Function Definitions    
    */

    GridPos GetLowestNeighbour(const std::map<GridPos, usize>& vertexCount, const GridPos& currentPos, usize& currentDirectionIndex)
    {
        GridPos neighbours[3] = {
            AddDirection(currentPos, ++currentDirectionIndex),
            AddDirection(currentPos, ++currentDirectionIndex),
            AddDirection(currentPos, ++currentDirectionIndex)
        };

        usize index = 0;
        usize min = 5; // Max is 4
        for (usize i = 0; i < 3; i++)
        {
            usize neighbourCount = vertexCount.at(neighbours[i]);
            if (neighbourCount < min)
            {
                min = neighbourCount;
                index = i;
            }
        }

        if (min == 2 || min == 3)
            return neighbours[index];

        return AddDirection(currentPos, currentDirectionIndex);
    }

    void TraverseR(const std::map<GridPos, usize>& vertexCount, Shape& vertices, GridPos& currentPos, usize& dIndex)
    {
        // Completed the loop
        if (!vertices.empty() && currentPos == vertices[0])
            return;

        usize currentCount = vertexCount.at(currentPos);
        if (currentCount == 2)
        {
            vertices.push_back(currentPos);
            currentPos = AddDirection(currentPos, (++dIndex % 4));
            TraverseR(vertexCount, vertices, currentPos, dIndex);
            return;
        }
        if (currentCount == 3)
        {
            TraverseR(vertexCount, vertices, currentPos, dIndex);
            return;
        }
        if (currentCount == 4)
        {
            vertices.push_back(currentPos);
            currentPos = AddDirection(currentPos, ((dIndex + 3) % 4));
            TraverseR(vertexCount, vertices, currentPos, dIndex);
            return;
        }

        LAB_CORE_ASSERT(false, "Invalid number of vertices found!");
    }
}