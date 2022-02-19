#pragma once
#include <memory>

#include <glm/glm.hpp>

#include <glutil/Matrix2D.h>
#include <glutil/Mesh.h>
#include <glutil/Shader.h>


class Shape2D
{
public:
    bool isHighlighted = false;
    bool isSelected = false;

    std::shared_ptr<Matrix2D> GetModel();
    void SetModel(const std::shared_ptr<Matrix2D>& matrix);
    virtual bool Contains(glm::vec2 pt) = 0;
    virtual void Draw(Shader& shader) = 0;

protected:
    std::shared_ptr<Matrix2D> m_model = std::make_shared<Matrix2D>();
};


class Rect : public Shape2D
{
public:
    Rect(const std::shared_ptr<Mesh>& mesh, glm::vec4 colour=glm::vec4(1));
    virtual bool Contains(glm::vec2 pt);
    virtual void Draw(Shader& shader);
    void SetColour(glm::vec4 col);
    glm::vec2 Min();
    glm::vec2 Max();
    // TODO: This is a bit weird, and breaks if the model is modified independently
    void SetCorners(glm::vec2 pos);
    void SetEndCorner(glm::vec2 pos);

protected:
    std::shared_ptr<Mesh> m_mesh;
    glm::vec4 m_colour;
    glm::vec2 startCorner;
    glm::vec2 endCorner;
};
