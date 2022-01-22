#include <model/Bounds.h>


Bounds2D::Bounds2D() {}
Bounds2D::Bounds2D(glm::vec2 min, glm::vec2 max) : min(min), max(max) {}

glm::vec2 Bounds2D::Center() const { return min + (max - min) * 0.5f; }
glm::vec2 Bounds2D::Size() const { return max - min; }
Bounds2D Bounds2D::Merge(const Bounds2D& bounds)
{
    return {glm::vec2(std::min(min.x, bounds.min.x)), glm::vec2(std::max(max.y, bounds.max.y))};
}
