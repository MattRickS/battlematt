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
    void DeserializeScene(nlohmann::json& json, Scene& scene);
    std::shared_ptr<Scene> DeserializeScene(nlohmann::json& json);
    std::shared_ptr<Token> DeserializeToken(nlohmann::json& json);

private:
    std::shared_ptr<Resources> m_resources;
};
