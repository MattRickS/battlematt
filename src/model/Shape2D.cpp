#include <glm/glm.hpp>

#include <glutil/Matrix2D.h>
#include <glutil/Shader.h>
#include <model/Shape2D.h>


// Shape2D
std::shared_ptr<Matrix2D> Shape2D::GetModel() { return m_model; }
void Shape2D::SetModel(const std::shared_ptr<Matrix2D>& matrix) { m_model = matrix; }
void Shape2D::SetVisibility(ShapeVisibility visibility) { m_visibility = visibility; }
ShapeVisibility Shape2D::GetVisibility() { return m_visibility; }
bool Shape2D::IsVisibleTo(ShapeVisibility visibility) { return (visibility & m_visibility) != ShapeVisibility::None; }

// Rect
Rect::Rect(const std::shared_ptr<Mesh>& mesh, glm::vec4 colour) : m_mesh(mesh), m_colour(colour) {}

bool Rect::Contains(glm::vec2 pt)
{
    // TODO: Account for rotation, being lazy atm
    glm::vec2 lo = m_model->GetPos() - m_model->GetScale() * 0.5f;
    glm::vec2 hi = m_model->GetPos() + m_model->GetScale() * 0.5f;
    return lo.x <= pt.x && hi.x >= pt.x && lo.y <= pt.y && hi.y >= pt.y;
}

void Rect::Draw(Shader& shader)
{
    shader.use();
    shader.setMat4("model", *m_model->Value());
    shader.setFloat4("colour", m_colour.x, m_colour.y, m_colour.z, m_colour.w);
    m_mesh->Draw(shader);
}


void Rect::SetColour(glm::vec4 col)
{
    m_colour = col;
}

glm::vec2 Rect::Min() { return glm::vec2(std::min(startCorner.x, endCorner.x), std::min(startCorner.y, endCorner.y)); }
glm::vec2 Rect::Max() { return glm::vec2(std::max(startCorner.x, endCorner.x), std::max(startCorner.y, endCorner.y)); }

void Rect::SetCorners(glm::vec2 pos)
{
    startCorner = endCorner = pos;
    m_model->SetPos(pos);
    m_model->SetScalef(0.0f);
}

void Rect::SetEndCorner(glm::vec2 pos)
{
    endCorner = pos;
    glm::vec2 half = (endCorner - startCorner) * 0.5f;
    m_model->SetPos(startCorner + half);
    m_model->SetScale(half);
}
