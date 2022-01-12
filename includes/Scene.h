#pragma once
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <json.hpp>

#include <BGImage.h>
#include <Camera.h>
#include <Grid.h>
#include <Matrix2D.h>
#include <Resources.h>
#include <Shader.h>
#include <Token.h>

class Scene
{
public:
    glm::vec4 bgColor = glm::vec4(0, 0, 0, 1);
    BGImage background;
    Grid grid;
    Camera* camera;
    std::vector<Token> tokens;

    Scene(std::shared_ptr<Resources> resources, Camera* camera, std::string bgPath="");
    void AddToken();
    void AddToken(std::string iconPath, glm::vec3 pos=glm::vec3(0));
    void AddToken(std::string iconPath, Matrix2D matrix);
    void RemoveTokens(std::vector<Token*> toRemove);
    void Draw();
    nlohmann::json Serialize() const;
    void Deserialize(nlohmann::json json);
    // TODO: Move this off this class
    void Save(std::string path);
    void Load(std::string path);

private:
    std::shared_ptr<Resources> m_resources;
};