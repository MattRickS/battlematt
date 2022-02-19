#pragma once
#include <memory>

#include <glm/glm.hpp>

#include <glutil/Matrix2D.h>
#include <glutil/Mesh.h>
#include <glutil/Shader.h>


enum class ShapeVisibility
{
    None = 0,
    Host = 1 << 0,
    Presentation = 1 << 1,
    All = Host | Presentation
};

inline ShapeVisibility operator~ (ShapeVisibility a) { return (ShapeVisibility)~(int)a; }
inline ShapeVisibility operator| (ShapeVisibility a, ShapeVisibility b) { return (ShapeVisibility)((int)a | (int)b); }
inline ShapeVisibility operator& (ShapeVisibility a, ShapeVisibility b) { return (ShapeVisibility)((int)a & (int)b); }
inline ShapeVisibility operator^ (ShapeVisibility a, ShapeVisibility b) { return (ShapeVisibility)((int)a ^ (int)b); }
inline ShapeVisibility& operator|= (ShapeVisibility& a, ShapeVisibility b) { return (ShapeVisibility&)((int&)a |= (int)b); }
inline ShapeVisibility& operator&= (ShapeVisibility& a, ShapeVisibility b) { return (ShapeVisibility&)((int&)a &= (int)b); }
inline ShapeVisibility& operator^= (ShapeVisibility& a, ShapeVisibility b) { return (ShapeVisibility&)((int&)a ^= (int)b); }

class Shape2D
{
public:
    bool isHighlighted = false;
    bool isSelected = false;

    std::shared_ptr<Matrix2D> GetModel();
    void SetModel(const std::shared_ptr<Matrix2D>& matrix);
    virtual bool Contains(glm::vec2 pt) = 0;
    virtual void Draw(Shader& shader) = 0;
    void SetVisibility(ShapeVisibility visibility);
    ShapeVisibility GetVisibility();
    bool IsVisibleTo(ShapeVisibility visibility);

protected:
    std::shared_ptr<Matrix2D> m_model = std::make_shared<Matrix2D>();
    ShapeVisibility m_visibility = ShapeVisibility::All;
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
