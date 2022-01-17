#include <glm/glm.hpp>

#include <glutil/Matrix2D.h>
#include <glutil/Shader.h>
#include <model/Shape2D.h>


// Shape2D
Matrix2D* Shape2D::GetModel() { return &m_model; }
void Shape2D::SetModel(Matrix2D matrix) { m_model = matrix; }

bool Shape2D::Contains(glm::vec2 pt) { return Contains(pt.x, pt.y); }

// Rect
Rect::Rect(std::shared_ptr<Mesh> mesh) : m_mesh(mesh) {}

bool Rect::Contains(float x, float y)
{
    // TODO: Account for rotation, being lazy atm
    glm::vec2 lo = m_model.GetPos() - m_model.GetScale();
    glm::vec2 hi = m_model.GetPos() + m_model.GetScale();
    return lo.x >= x && hi.x <= x && lo.y >= y && hi.y <= y;
}

void Rect::Draw(Shader& shader)
{
    m_mesh->Draw(shader);
}