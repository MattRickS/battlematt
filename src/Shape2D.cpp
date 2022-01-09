#include <glm/glm.hpp>

#include <Matrix2D.h>
#include <Shader.h>
#include <Shape2D.h>


// Shape2D
Matrix2D* Shape2D::GetModel() { return &m_model; }

bool Shape2D::Contains(glm::vec2 pt) { return Contains(pt.x, pt.y); }

// Rect
Rect::Rect() : Quad() {}

bool Rect::Contains(float x, float y)
{
    // TODO: Account for rotation, being lazy atm
    glm::vec2 lo = m_model.GetPos() - m_model.GetScale();
    glm::vec2 hi = m_model.GetPos() + m_model.GetScale();
    return lo.x >= x && hi.x <= x && lo.y >= y && hi.y <= y;
}

void Rect::Draw(Shader& shader)
{
    Quad::Draw(shader);
}