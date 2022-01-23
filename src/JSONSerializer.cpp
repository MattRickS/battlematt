#include <algorithm>
#include <memory>
#include <vector>

#include <json.hpp>

#include <Resources.h>
#include <glutil/Camera.h>
#include <glutil/Matrix2D.h>
#include <model/BGImage.h>
#include <model/Grid.h>
#include <model/Overlays.h>
#include <model/Scene.h>
#include <model/Token.h>

#include <JSONSerializer.h>


JSONSerializer::JSONSerializer(std::shared_ptr<Resources> resources) : m_resources(resources) {}

// Camera
bool JSONSerializer::SerializeCamera(std::shared_ptr<Camera> camera, nlohmann::json& json)
{
    json["pos"] = {camera->Position.x, camera->Position.y, camera->Position.z};
    json["focal"] = camera->Focal;
    return true;
}

std::shared_ptr<Camera> JSONSerializer::DeserializeCamera(nlohmann::json& json)
{
    return std::make_shared<Camera>(
        glm::vec3(json["pos"][0], json["pos"][1], json["pos"][2]), glm::vec3(0.0f, 0.0f, -1.0f), true, json["focal"]
    );
}

// Grid
bool JSONSerializer::SerializeGrid(std::shared_ptr<Grid> grid, nlohmann::json& json)
{
    json["scale"] = grid->GetScale();
    return true;
}

std::shared_ptr<Grid> JSONSerializer::DeserializeGrid(nlohmann::json& json)
{
    std::shared_ptr<Grid> grid = std::make_shared<Grid>(
        m_resources->GetMesh(Resources::MeshType::Quad2),
        m_resources->GetShader(Resources::ShaderType::Grid)
    );
    grid->SetScale(json["scale"]);
    return grid;
}

// Image
bool JSONSerializer::SerializeImage(std::shared_ptr<BGImage> image, nlohmann::json& json)
{
    json["texture"] = image->GetImage()->filename;
    nlohmann::json matrix;
    SerializeMatrix2D(image->GetModel(), matrix);
    json["matrix2D"] = matrix;
    return true;
}

std::shared_ptr<BGImage> JSONSerializer::DeserializeImage(nlohmann::json& json)
{
    std::shared_ptr<BGImage> image = std::make_shared<BGImage>(
        m_resources->GetMesh(Resources::MeshType::Quad),
        m_resources->GetTexture(std::string(json["texture"]))
    );
    image->SetModel(DeserializeMatrix2D(json["matrix2D"]));
    return image;
}

// Matrix2D
bool JSONSerializer::SerializeMatrix2D(std::shared_ptr<Matrix2D> matrix, nlohmann::json& json)
{
    json["pos"] = {matrix->GetPos().x, matrix->GetPos().y};
    json["scale"] = {matrix->GetScale().x, matrix->GetScale().y};
    json["rotation"] = matrix->GetRotation();
    return true;
}

std::shared_ptr<Matrix2D> JSONSerializer::DeserializeMatrix2D(nlohmann::json& json)
{
    return std::make_shared<Matrix2D>(
        glm::vec2(json["pos"][0], json["pos"][1]),
        glm::vec2(json["scale"][0], json["scale"][1]),
        json["rotation"]
    );
}

// Token
bool JSONSerializer::SerializeToken(std::shared_ptr<Token> token, nlohmann::json& json)
{
    nlohmann::json matrix;
    SerializeMatrix2D(token->GetModel(), matrix);
    json["matrix2D"] = matrix;
    json["texture"] = token->GetIcon()->filename;
    json["name"] = token->GetName();
    json["borderColour"] = {token->GetBorderColor().x, token->GetBorderColor().y, token->GetBorderColor().z, token->GetBorderColor().w};
    json["borderWidth"] = token->GetBorderWidth();
    json["statuses"] = token->GetStatuses().to_string();
    json["xstatus"] = token->GetXStatus();
    json["opacity"] = token->GetOpacity();

    return true;
}

std::shared_ptr<Token> JSONSerializer::DeserializeToken(nlohmann::json& json)
{
    std::shared_ptr<Token> token = std::make_shared<Token>(
        m_resources->GetMesh(Resources::MeshType::Quad),
        m_resources->GetTexture(std::string(json["texture"])),
        json["name"]
    );
    token->SetModel(DeserializeMatrix2D(json["matrix2D"]));
    token->SetBorderWidth(json["borderWidth"]);
    token->SetBorderColor(glm::vec4(
        json["borderColour"][0], json["borderColour"][1], json["borderColour"][2], json["borderColour"][3]
    ));
    // Status properties added together
    if (json.contains("statuses"))
    {
        TokenStatuses statuses(static_cast<std::string>(json["statuses"]));
        token->SetStatuses(statuses);
        token->SetOpacity(json["opacity"]);
        token->SetXStatus(json["xstatus"]);
    }
    return token;
}

// Scene
bool JSONSerializer::SerializeScene(std::shared_ptr<Scene> scene, nlohmann::json& json)
{
    std::vector<nlohmann::json> jtokens(scene->tokens.size());
    uint i = 0;
    std::for_each(scene->tokens.begin(), scene->tokens.end(),
                  [this, &i, &jtokens](const std::shared_ptr<Token> token){ SerializeToken(token, jtokens[i++]); });
    json["tokens"] = jtokens;

    std::vector<nlohmann::json> jimages(scene->images.size());
    i = 0;
    std::for_each(scene->images.begin(), scene->images.end(),
                  [this, &i, &jimages](const std::shared_ptr<BGImage> image){ SerializeImage(image, jimages[i++]); });
    json["images"] = jimages;

    nlohmann::json jcamera;
    SerializeCamera(scene->camera, jcamera);
    json["camera"] = jcamera;

    nlohmann::json jgrid;
    SerializeGrid(scene->grid, jgrid);
    json["grid"] = jgrid;

    return true;
}

void JSONSerializer::DeserializeScene(nlohmann::json& json, Scene& scene)
{
    scene.camera = DeserializeCamera(json["camera"]);
    scene.grid = DeserializeGrid(json["grid"]);

    nlohmann::json jimages = json["images"];
    scene.images.reserve(jimages.size());
    std::for_each(jimages.begin(), jimages.end(),
                  [this, &scene](nlohmann::json& jimage){ scene.images.push_back(DeserializeImage(jimage)); });

    nlohmann::json jtokens = json["tokens"];
    scene.tokens.reserve(jtokens.size());
    std::for_each(jtokens.begin(), jtokens.end(),
                  [this, &scene](nlohmann::json& jtoken){ scene.tokens.push_back(DeserializeToken(jtoken)); });
}

std::shared_ptr<Scene> JSONSerializer::DeserializeScene(nlohmann::json& json)
{
    std::shared_ptr<Scene> scene = std::make_shared<Scene>(m_resources);
    DeserializeScene(json, *scene);
    return scene;
}

nlohmann::json JSONSerializer::SerializeScene(std::shared_ptr<Scene> scene)
{
    nlohmann::json json;
    SerializeScene(scene, json);
    return json;
}
