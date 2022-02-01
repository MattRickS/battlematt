#include <model/Bounds.h>


Bounds2D::Bounds2D() {}
Bounds2D::Bounds2D(glm::vec2 min, glm::vec2 max) : min(min), max(max) {}
Bounds2D::Bounds2D(float minx, float miny, float maxx, float maxy) : min(minx, miny), max(maxx, maxy) {}

glm::vec2 Bounds2D::Center() const { return min + (max - min) * 0.5f; }
glm::vec2 Bounds2D::Size() const { return max - min; }
Bounds2D Bounds2D::Merge(const Bounds2D& bounds)
{
    return {glm::vec2(std::min(min.x, bounds.min.x)), glm::vec2(std::max(max.y, bounds.max.y))};
}

float Bounds2D::MinX() { return std::min(min.x, max.x); }
float Bounds2D::MaxX() { return std::max(min.x, max.x); }
float Bounds2D::MinY() { return std::min(min.y, max.y); }
float Bounds2D::MaxY() { return std::max(min.y, max.y); }
