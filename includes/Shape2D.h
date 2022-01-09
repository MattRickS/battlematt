#pragma once
#include <glm/glm.hpp>

#include <Matrix2D.h>
#include <Primitives.h>
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


class Rect : public Shape2D, public Quad
{
public:
    Rect();
    virtual bool Contains(float x, float y);
    virtual void Draw(Shader& shader);
};
