#pragma once
#include <memory>

#include <glm/glm.hpp>

#include <Matrix2D.h>
#include <Mesh.h>
#include <Shader.h>


class Shape2D
{
public:
    Matrix2D* GetModel();
    void SetModel(Matrix2D matrix);
    virtual bool Contains(float x, float y) = 0;
    virtual bool Contains(glm::vec2 pt);
    virtual void Draw(Shader& shader) = 0;

protected:
    Matrix2D m_model;
};


class Rect : public Shape2D
{
public:
    Rect(std::shared_ptr<Mesh> mesh);
    virtual bool Contains(float x, float y);
    virtual void Draw(Shader& shader);

private:
    std::shared_ptr<Mesh> m_mesh;
};
