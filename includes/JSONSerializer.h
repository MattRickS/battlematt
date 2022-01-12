#pragma once
#include <algorithm>
#include <memory>
#include <vector>

#include <json.hpp>

#include <BGImage.h>
#include <Camera.h>
#include <Grid.h>
#include <Matrix2D.h>
#include <Overlays.h>
#include <Resources.h>
#include <Scene.h>
#include <Token.h>


class JSONSerializer
{
public:
    JSONSerializer(std::shared_ptr<Resources> resources);

    bool SerializeCamera(std::shared_ptr<Camera> camera, nlohmann::json& json);
    bool SerializeGrid(std::shared_ptr<Grid> grid, nlohmann::json& json);
    bool SerializeImage(std::shared_ptr<BGImage> image, nlohmann::json& json);
    bool SerializeMatrix2D(Matrix2D* matrix, nlohmann::json& json);
    // bool SerializeOverlay(std::shared_ptr<Overlay> overlay, nlohmann::json& json);
    bool SerializeScene(std::shared_ptr<Scene> scene, nlohmann::json& json);
    bool SerializeToken(std::shared_ptr<Token> token, nlohmann::json& json);

    nlohmann::json SerializeScene(std::shared_ptr<Scene> scene);

    std::shared_ptr<Camera> DeserializeCamera(nlohmann::json& json);
    std::shared_ptr<Grid> DeserializeGrid(nlohmann::json& json);
    std::shared_ptr<BGImage> DeserializeImage(nlohmann::json& json);
    std::shared_ptr<Matrix2D> DeserializeMatrix2D(nlohmann::json& json);
    // std::shared_ptr<Overlay> DeserializeOverlay(nlohmann::json& json);
    std::shared_ptr<Scene> DeserializeScene(nlohmann::json& json);
    std::shared_ptr<Token> DeserializeToken(nlohmann::json& json);

private:
    std::shared_ptr<Resources> m_resources;
};

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
        glm::vec3(json["pos"]), glm::vec3(0.0f, 0.0f, -1.0f), true, json["focal"]
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
    std::shared_ptr<Grid> grid = std::make_shared<Grid>();
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
    image->SetModel(*DeserializeMatrix2D(json["matrix2D"]));
    return image;
}

// Matrix2D
bool JSONSerializer::SerializeMatrix2D(Matrix2D* matrix, nlohmann::json& json)
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
    json["name"] = token->name;
    json["borderColour"] = {token->borderColor.x, token->borderColor.y, token->borderColor.z, token->borderColor.w};
    json["borderWidth"] = token->borderWidth;

    return true;
}

std::shared_ptr<Token> JSONSerializer::DeserializeToken(nlohmann::json& json)
{
    std::shared_ptr<Token> token = std::make_shared<Token>(
        m_resources->GetMesh(Resources::MeshType::Quad),
        m_resources->GetTexture(std::string(json["texture"])),
        json["name"]
    );
    token->SetModel(*DeserializeMatrix2D(json["matrix2D"]));
    token->borderWidth = json["borderWidth"];
    token->borderColor = glm::vec4(json["borderColour"]);
    return token;
}

// Scene
bool JSONSerializer::SerializeScene(std::shared_ptr<Scene> scene, nlohmann::json& json)
{
    std::vector<nlohmann::json> jtokens(scene->tokens.size());
    uint i = 0;
    std::transform(scene->tokens.begin(), scene->tokens.end(), jtokens.begin(),
                   [this, &i, &jtokens](const std::shared_ptr<Token> token){ return SerializeToken(token, jtokens[i++]); });
    json["tokens"] = jtokens;

    std::vector<nlohmann::json> jimages(scene->tokens.size());
    uint i = 0;
    std::transform(scene->backgrounds.begin(), scene->backgrounds.end(), jimages.begin(),
                   [this, &i, &jimages](const std::shared_ptr<BGImage> image){ return SerializeImage(image, jimages[i++]); });
    json["images"] = jimages;

    nlohmann::json jcamera;
    SerializeCamera(scene->camera, jcamera);
    json["camera"] = jcamera;

    nlohmann::json jgrid;
    SerializeGrid(scene->grid, jgrid);
    json["grid"] = jgrid;

    return true;
}

std::shared_ptr<Scene> JSONSerializer::DeserializeScene(nlohmann::json& json)
{
    std::shared_ptr<Scene> scene = std::make_shared<Scene>(m_resources, DeserializeCamera(json["camera"]));
    scene->grid = DeserializeGrid(json["grid"]);

    nlohmann::json jimages = json["images"];
    scene->backgrounds.reserve(jimages.size());
    std::for_each(jimages.begin(), jimages.end(),
                  [this, &scene](nlohmann::json& jimage){ scene->backgrounds.push_back(DeserializeImage(jimage)); });

    nlohmann::json jtokens = json["tokens"];
    scene->tokens.reserve(jtokens.size());
    std::for_each(jtokens.begin(), jtokens.end(),
                  [this, &scene](nlohmann::json& jtoken){ scene->tokens.push_back(DeserializeToken(jtoken)); });

    return scene;
}

nlohmann::json JSONSerializer::SerializeScene(std::shared_ptr<Scene> scene)
{
    nlohmann::json json;
    SerializeScene(scene, json);
    return json;
}
