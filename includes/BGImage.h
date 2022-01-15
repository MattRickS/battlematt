#pragma once
#include <memory>

#include <glm/glm.hpp>

#include <Matrix2D.h>
#include <Mesh.h>
#include <Shape2D.h>
#include <Texture.h>


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
