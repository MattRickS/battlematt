#pragma once
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include <Resources.h>
#include <glutil/Camera.h>
#include <glutil/Shader.h>
#include <model/BGImage.h>
#include <model/Grid.h>
#include <model/Overlays.h>
#include <model/Token.h>

struct Bounds
{
    glm::vec2 min = glm::vec2(0);
    glm::vec2 max = glm::vec2(0);

    Bounds() {}
    Bounds(glm::vec2 min, glm::vec2 max) : min(min), max(max) {}

    glm::vec2 Center() const { return min + (max - min) * 0.5f; }
    glm::vec2 Size() const { return max - min; }
};


class Scene
{
public:
    glm::vec4 bgColor = glm::vec4(0, 0, 0, 1);
    std::vector<std::shared_ptr<BGImage>> images;
    std::vector<std::shared_ptr<Token>> tokens;
    std::vector<std::shared_ptr<Overlay>> overlays;
    std::shared_ptr<Grid> grid = nullptr;
    std::shared_ptr<Camera> camera = nullptr;
    std::string sourceFile;

    Scene(std::shared_ptr<Resources> resources);
    void AddImage();
    void AddImage(std::string path);
    void AddImage(const std::shared_ptr<BGImage>& image);
    void AddToken();
    void AddToken(std::string path);
    void AddToken(const std::shared_ptr<Token>& token);
    void RemoveOverlay(std::shared_ptr<Overlay> overlay);
    void RemoveTokens(std::vector<std::shared_ptr<Token>> toRemove);
    void RemoveImages(std::vector<std::shared_ptr<BGImage>> toRemove);
    Bounds GetBounds();
    void Draw();

private:
    std::shared_ptr<Resources> m_resources;
};