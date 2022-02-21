#include <algorithm>
#include <memory>
#include <vector>

#include <json.hpp>

#include <Resources.h>
#include <glutil/Camera.h>
#include <glutil/Matrix2D.h>
#include <model/BGImage.h>
#include <model/Grid.h>
#include <model/Scene.h>
#include <model/Token.h>

#include <JSONSerializer.h>


JSONSerializer::JSONSerializer(std::shared_ptr<Resources> resources) : m_resources(resources) {}

// Camera
bool JSONSerializer::SerializeCamera(const std::shared_ptr<Camera>& camera, nlohmann::json& json)
{
    json["pos"] = {camera->Position.x, camera->Position.y, camera->Position.z};
    json["focal"] = camera->Focal;
    json["name"] = camera->GetName();
    return true;
}

std::shared_ptr<Camera> JSONSerializer::DeserializeCamera(nlohmann::json& json)
{
    auto camera = std::make_shared<Camera>(
        glm::vec3(json["pos"][0], json["pos"][1], json["pos"][2]), glm::vec3(0.0f, 0.0f, -1.0f), true, json["focal"]
    );
    if (json.contains("name"))
        camera->SetName(json["name"]);
    return camera;
}

// Grid
bool JSONSerializer::SerializeGrid(const std::shared_ptr<Grid>& grid, nlohmann::json& json)
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
bool JSONSerializer::SerializeImage(const std::shared_ptr<BGImage>& image, nlohmann::json& json)
{
    json["texture"] = image->GetImage()->filename;
    nlohmann::json matrix;
    SerializeMatrix2D(image->GetModel(), matrix);
    json["matrix2D"] = matrix;
    json["visibility"] = image->GetVisibilities().to_ulong();
    return true;
}

nlohmann::json JSONSerializer::SerializeImages(const std::vector<std::shared_ptr<BGImage>>& images, bool selectedOnly)
{
    nlohmann::json jimages = nlohmann::json::array();
    unsigned int i = 0;
    std::for_each(images.begin(), images.end(),
                  [this, &i, &jimages, &selectedOnly](const std::shared_ptr<BGImage> image){
                      if (image->isSelected || !selectedOnly)
                          SerializeImage(image, jimages[i++]);
                  });
    return jimages;
}

std::shared_ptr<BGImage> JSONSerializer::DeserializeImage(nlohmann::json& json)
{
    std::shared_ptr<BGImage> image = std::make_shared<BGImage>(
        m_resources->GetMesh(Resources::MeshType::Quad),
        m_resources->GetTexture(std::string(json["texture"]))
    );
    image->SetModel(DeserializeMatrix2D(json["matrix2D"]));
    if (json.contains("visibility"))
        image->SetVisibilities((unsigned int)json["visibility"]);
    return image;
}

// Matrix2D
bool JSONSerializer::SerializeMatrix2D(const std::shared_ptr<Matrix2D>& matrix, nlohmann::json& json)
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
bool JSONSerializer::SerializeToken(const std::shared_ptr<Token>& token, nlohmann::json& json)
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
    json["visibility"] = token->GetVisibilities().to_ulong();

    return true;
}

nlohmann::json JSONSerializer::SerializeTokens(const std::vector<std::shared_ptr<Token>>& tokens, bool selectedOnly)
{
    nlohmann::json jtokens = nlohmann::json::array();
    uint i = 0;
    std::for_each(tokens.begin(), tokens.end(),
                  [this, &i, &jtokens, &selectedOnly](const std::shared_ptr<Token> token){
                      if (token->isSelected || !selectedOnly)
                          SerializeToken(token, jtokens[i++]);
                  });
    return jtokens;
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
    if (json.contains("visibility"))
        token->SetVisibilities((unsigned int)json["visibility"]);
    return token;
}

// Scene
bool JSONSerializer::SerializeScene(const std::shared_ptr<Scene>& scene, nlohmann::json& json)
{
    json["defaultVisibility"] = scene->GetDefaultVisibilities().to_ulong();

    json["images"] = SerializeImages(scene->images);
    json["imagesLocked"] = scene->GetImagesLocked();

    json["tokens"] = SerializeTokens(scene->tokens);
    json["tokensLocked"] = scene->GetTokensLocked();

    nlohmann::json jcameras = nlohmann::json::array();
    uint i = 0;
    std::for_each(scene->cameras.begin(), scene->cameras.end(),
                  [this, &i, &jcameras](const std::shared_ptr<Camera> camera){ SerializeCamera(camera, jcameras[i++]); });
    json["cameras"] = jcameras;

    nlohmann::json jviews = nlohmann::json::array();
    i = 0;
    for (const auto& pair: scene->views)
    {
        if (pair.second != nullptr)
        {
            auto it = std::find(scene->cameras.begin(), scene->cameras.end(), pair.second);
            jviews[i++] = {{"id", pair.first}, {"index", it - scene->cameras.begin()}};
        }
    }
    json["views"] = jviews;

    nlohmann::json jgrid;
    SerializeGrid(scene->grid, jgrid);
    json["grid"] = jgrid;

    return true;
}

void JSONSerializer::DeserializeScene(nlohmann::json& json, Scene& scene)
{
    if (json.contains("cameras"))
    {
        nlohmann::json jcameras = json["cameras"];
        scene.images.reserve(jcameras.size());
        std::for_each(jcameras.begin(), jcameras.end(),
                      [this, &scene](nlohmann::json& jcamera){ scene.cameras.push_back(DeserializeCamera(jcamera)); });
    }
    if (json.contains("camera"))
        scene.cameras.push_back(DeserializeCamera(json["camera"]));

    if (json.contains("views"))
    {
        nlohmann::json jviews = json["views"];
        std::for_each(jviews.begin(), jviews.end(),
                      [this, &scene](nlohmann::json& jview){ scene.SetViewCamera(jview["id"], scene.cameras[jview["index"]]); });
    }

    if (json.contains("grid"))
        scene.grid = DeserializeGrid(json["grid"]);

    if (json.contains("images"))
    {
        nlohmann::json jimages = json["images"];
        scene.images.reserve(jimages.size());
        std::for_each(jimages.begin(), jimages.end(),
                    [this, &scene](nlohmann::json& jimage){ scene.images.push_back(DeserializeImage(jimage)); });
    }
    if (json.contains("imagesLocked"))
        scene.SetImagesLocked(json["imagesLocked"]);

    if (json.contains("tokens"))
    {
        nlohmann::json jtokens = json["tokens"];
        scene.tokens.reserve(jtokens.size());
        std::for_each(jtokens.begin(), jtokens.end(),
                    [this, &scene](nlohmann::json& jtoken){ scene.tokens.push_back(DeserializeToken(jtoken)); });
    }
    if (json.contains("tokensLocked"))
        scene.SetTokensLocked(json["tokensLocked"]);

    if (json.contains("defaultVisibility"))
        scene.SetDefaultVisibilities((unsigned int)json["defaultVisibility"]);
}

std::shared_ptr<Scene> JSONSerializer::DeserializeScene(nlohmann::json& json)
{
    std::shared_ptr<Scene> scene = std::make_shared<Scene>(m_resources);
    DeserializeScene(json, *scene);
    return scene;
}

std::shared_ptr<Scene> JSONSerializer::DeserializeScene(const std::string& text)
{
    nlohmann::json json = nlohmann::json::parse(text);
    return DeserializeScene(json);
}

nlohmann::json JSONSerializer::SerializeScene(const std::shared_ptr<Scene>& scene)
{
    nlohmann::json json;
    SerializeScene(scene, json);
    return json;
}

nlohmann::json JSONSerializer::SerializeScene(const std::shared_ptr<Scene>& scene, SerializeFlag flags)
{
    nlohmann::json json;
    if (bool(flags & SerializeFlag::Token) || bool(flags & SerializeFlag::All))
    {
        auto serialized = SerializeTokens(scene->tokens, bool(flags & SerializeFlag::Selected) && !bool(flags & SerializeFlag::All));
        if (!serialized.empty())
            json["tokens"] = serialized;
    }

    if (bool(flags & SerializeFlag::Image) || bool(flags & SerializeFlag::All))
    {
        auto serialized = SerializeImages(scene->images, bool(flags & SerializeFlag::Selected) && !bool(flags & SerializeFlag::All));
        if (!serialized.empty())
            json["images"] = serialized;
    }

    if (bool(flags & SerializeFlag::Camera) || bool(flags & SerializeFlag::All))
    {
        nlohmann::json jcameras = nlohmann::json::array();
        uint i = 0;
        std::for_each(scene->cameras.begin(), scene->cameras.end(),
                    [this, &i, &jcameras](const std::shared_ptr<Camera> camera){ SerializeCamera(camera, jcameras[i++]); });
        json["cameras"] = jcameras;
    }

    if (bool(flags & SerializeFlag::Grid) || bool(flags & SerializeFlag::All))
    {
        nlohmann::json jgrid;
        SerializeGrid(scene->grid, jgrid);
        json["grid"] = jgrid;
    }

    if (bool(flags & SerializeFlag::View) || bool(flags & SerializeFlag::All))
    {
        nlohmann::json jviews = nlohmann::json::array();
        uint i = 0;
        for (const auto& pair: scene->views)
        {
            if (pair.second != nullptr)
            {
                auto it = std::find(scene->cameras.begin(), scene->cameras.end(), pair.second);
                jviews[i++] = {{"id", pair.first}, {"index", it - scene->cameras.begin()}};
            }
        }
        json["views"] = jviews;
    }

    return json;
}
