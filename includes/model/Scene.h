#pragma once
#include <string>
#include <unordered_map>
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

// ViewIDs are a lookup for which camera is being used for what purpose
// A scene will always have a primary view, but may in future have a
// presentation view and/or per player views.
typedef unsigned int ViewID;
const ViewID HOST_VIEW = 0;
const ViewID PRESENTATION_VIEW = 1;

class Scene
{
public:
    glm::vec4 bgColor = glm::vec4(0, 0, 0, 1);
    std::vector<std::shared_ptr<BGImage>> images;
    std::vector<std::shared_ptr<Token>> tokens;
    std::vector<std::shared_ptr<Overlay>> overlays;
    std::shared_ptr<Grid> grid = nullptr;
    std::vector<std::shared_ptr<Camera>> cameras;
    std::unordered_map<ViewID, std::shared_ptr<Camera>> views;
    std::string sourceFile;

    Scene(std::shared_ptr<Resources> resources);
    void AddCamera(const std::shared_ptr<Camera>& camera);
    void AddImage();
    void AddImage(std::string path);
    void AddImage(const std::shared_ptr<BGImage>& image);
    void AddToken();
    void AddToken(std::string path);
    void AddToken(const std::shared_ptr<Token>& token);
    void RemoveOverlay(std::shared_ptr<Overlay> overlay);
    void RemoveTokens(std::vector<std::shared_ptr<Token>> toRemove);
    void RemoveImages(std::vector<std::shared_ptr<BGImage>> toRemove);
    bool RemoveCamera(const std::shared_ptr<Camera>& camera);
    bool IsEmpty();
    Bounds2D GetBounds();
    bool GetImagesLocked();
    void SetImagesLocked(bool locked);
    bool GetTokensLocked();
    void SetTokensLocked(bool locked);
    void Draw();

    void AddDefaultCamera();
    void SetViewCamera(ViewID id, const std::shared_ptr<Camera>& camera);
    const std::shared_ptr<Camera>& GetViewCamera(ViewID id);

private:
    std::shared_ptr<Resources> m_resources;
    bool m_lockImages = false;
    bool m_lockTokens = false;
    unsigned int m_primaryCamera = -1;
};