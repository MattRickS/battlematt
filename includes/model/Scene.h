#pragma once
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include <BGImage.h>
#include <Camera.h>
#include <Grid.h>
#include <Overlays.h>
#include <Resources.h>
#include <Shader.h>
#include <Token.h>

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
    void AddToken();
    void AddToken(std::string path);
    void AddToken(std::shared_ptr<Token> token);
    void RemoveOverlay(std::shared_ptr<Overlay> overlay);
    void RemoveTokens(std::vector<std::shared_ptr<Token>> toRemove);
    void Draw();

private:
    std::shared_ptr<Resources> m_resources;
};