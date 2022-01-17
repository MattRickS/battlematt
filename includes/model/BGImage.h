#pragma once
#include <memory>

#include <glm/glm.hpp>

#include <glutil/Matrix2D.h>
#include <glutil/Mesh.h>
#include <glutil/Shader.h>
#include <glutil/Texture.h>
#include <model/Shape2D.h>


class BGImage: public Rect
{
public:
    bool lockRatio = false;

    BGImage(std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> texture);
    void Draw(Shader &shader) override;
    std::shared_ptr<Texture> GetImage();
    void SetImage(std::shared_ptr<Texture> texture);

private:
    std::shared_ptr<Texture> m_texture;
};
