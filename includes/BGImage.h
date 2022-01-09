#pragma once
#include <string>

#include <glm/glm.hpp>
#include <json.hpp>

#include <Matrix2D.h>
#include <Shape2D.h>
#include <Texture.h>


class BGImage: public Rect
{
public:
    BGImage(std::string texturePath);
    void Draw(Shader &shader) override;
    std::string GetImage();
    void SetImage(std::string imagePath);
    nlohmann::json Serialize() const;
    void Deserialize(nlohmann::json json);

private:
    Texture* tex;
};
