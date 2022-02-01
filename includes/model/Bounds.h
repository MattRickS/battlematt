#pragma once
#include <vector>

#include <glm/glm.hpp>

#include <model/Shape2D.h>


class Bounds2D
{
public:
    glm::vec2 min = glm::vec2(0);
    glm::vec2 max = glm::vec2(0);

    Bounds2D();
    Bounds2D(glm::vec2 min, glm::vec2 max);
    Bounds2D(float minx, float miny, float maxx, float maxy);

    glm::vec2 Center() const;
    glm::vec2 Size() const;
    Bounds2D Merge(const Bounds2D& bounds);
    float MinX() { return std::min(min.x, max.x); }
    float MaxX() { return std::max(min.x, max.x); }
    float MinY() { return std::min(min.y, max.y); }
    float MaxY() { return std::max(min.y, max.y); }

    // TODO: Bounds should be a property in shapes, and this method exist elsewhere
    static Bounds2D BoundsForShapes(std::vector<std::shared_ptr<Shape2D>> shapes)
    {
        Bounds2D bounds;
        if (shapes.empty())
            return bounds;

        bounds.min = shapes[0]->GetModel()->GetPos() - shapes[0]->GetModel()->GetScale() * 0.5f;
        bounds.max = shapes[0]->GetModel()->GetPos() + shapes[0]->GetModel()->GetScale() * 0.5f;

        for (unsigned int i = 1; i < shapes.size(); i++)
        {
            // TODO: Doesn't account for rotation...
            glm::vec2 lo = shapes[i]->GetModel()->GetPos() - shapes[i]->GetModel()->GetScale() * 0.5f;
            glm::vec2 hi = shapes[i]->GetModel()->GetPos() + shapes[i]->GetModel()->GetScale() * 0.5f;
            bounds.min = glm::vec2(std::min(bounds.min.x, lo.x), std::min(bounds.min.y, lo.y));
            bounds.max = glm::vec2(std::max(bounds.max.x, hi.x), std::max(bounds.max.y, hi.y));
        }
        return bounds;
    }
};