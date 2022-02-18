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
    Rect(std::shared_ptr<Mesh> mesh);
    virtual bool Contains(glm::vec2 pt);
    virtual void Draw(Shader& shader);

protected:
    std::shared_ptr<Mesh> m_mesh;
};
