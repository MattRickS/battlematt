#pragma once
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include <Resources.h>
#include <glutil/Camera.h>
#include <glutil/Shader.h>
#include <model/BGImage.h>
#include <model/Bounds.h>
#include <model/Grid.h>
#include <model/Overlays.h>
#include <model/Token.h>


class Scene
{
public:
    glm::vec4 bgColor = glm::vec4(0, 0, 0, 1);
    std::vector<std::shared_ptr<BGImage>> images;
    std::vector<std::shared_ptr<Token>> tokens;
    std::vector<std::shared_ptr<Overlay>> overlays;
    std::shared_ptr<Grid> grid = nullptr;
    std::vector<std::shared_ptr<Camera>> cameras;
    std::string sourceFile;

    Scene(std::shared_ptr<Resources> resources);
    void AddCamera(std::shared_ptr<Camera> camera);
    void AddDefaultCamera();
    void AddImage();
    void AddImage(std::string path);
    void AddImage(const std::shared_ptr<BGImage>& image);
    void AddToken();
    void AddToken(std::string path);
    void AddToken(const std::shared_ptr<Token>& token);
    void RemoveOverlay(std::shared_ptr<Overlay> overlay);
    void RemoveTokens(std::vector<std::shared_ptr<Token>> toRemove);
    void RemoveImages(std::vector<std::shared_ptr<BGImage>> toRemove);
    void RemoveCameras(std::vector<std::shared_ptr<Camera>> toRemove);
    bool IsEmpty();
    Bounds2D GetBounds();
    bool GetImagesLocked();
    void SetImagesLocked(bool locked);
    bool GetTokensLocked();
    void SetTokensLocked(bool locked);
    void Draw();

private:
    std::shared_ptr<Resources> m_resources;
    bool m_lockImages = false;
    bool m_lockTokens = false;
};