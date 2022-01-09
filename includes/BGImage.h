#pragma once
#include <string>

#include <glm/glm.hpp>
#include <json.hpp>

#include <Matrix2D.h>
#include <Texture.h>
#include <Primitives.h>


class BGImage: public Quad
{
public:
    BGImage(std::string texturePath);
    Matrix2D* GetModel() { return &m_model; }
    void Draw(Shader &shader);
    std::string GetImage();
    void SetImage(std::string imagePath);
    nlohmann::json Serialize() const;
    void Deserialize(nlohmann::json json);

private:
    Texture* tex;
    Matrix2D m_model;
};
