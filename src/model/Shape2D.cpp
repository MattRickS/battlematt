#include <glm/glm.hpp>

#include <glutil/Matrix2D.h>
#include <glutil/Shader.h>
#include <model/Shape2D.h>


// Shape2D
std::shared_ptr<Matrix2D> Shape2D::GetModel() { return m_model; }
void Shape2D::SetModel(const std::shared_ptr<Matrix2D>& matrix) { m_model = matrix; }

// Rect
Rect::Rect(std::shared_ptr<Mesh> mesh) : m_mesh(mesh) {}

bool Rect::Contains(glm::vec2 pt)
{
    // TODO: Account for rotation, being lazy atm
    glm::vec2 lo = m_model->GetPos() - m_model->GetScale();
    glm::vec2 hi = m_model->GetPos() + m_model->GetScale();
    return lo.x >= pt.x && hi.x <= pt.x && lo.y >= pt.y && hi.y <= pt.y;
}

void Rect::Draw(Shader& shader)
{
    m_mesh->Draw(shader);
}