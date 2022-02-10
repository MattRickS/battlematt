#pragma once
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


enum class SerializeFlag
{
    All      = 1 << 0,
    Token    = 1 << 1,
    Image    = 1 << 2,
    Camera   = 1 << 3,
    Grid     = 1 << 4,
    View     = 1 << 5,
    Selected = 1 << 8
};

inline SerializeFlag operator~ (SerializeFlag a) { return (SerializeFlag)~(int)a; }
inline SerializeFlag operator| (SerializeFlag a, SerializeFlag b) { return (SerializeFlag)((int)a | (int)b); }
inline SerializeFlag operator& (SerializeFlag a, SerializeFlag b) { return (SerializeFlag)((int)a & (int)b); }
inline SerializeFlag operator^ (SerializeFlag a, SerializeFlag b) { return (SerializeFlag)((int)a ^ (int)b); }
inline SerializeFlag& operator|= (SerializeFlag& a, SerializeFlag b) { return (SerializeFlag&)((int&)a |= (int)b); }
inline SerializeFlag& operator&= (SerializeFlag& a, SerializeFlag b) { return (SerializeFlag&)((int&)a &= (int)b); }
inline SerializeFlag& operator^= (SerializeFlag& a, SerializeFlag b) { return (SerializeFlag&)((int&)a ^= (int)b); }


class JSONSerializer
{
public:
    JSONSerializer(std::shared_ptr<Resources> resources);

    bool SerializeCamera(const std::shared_ptr<Camera>& camera, nlohmann::json& json);
    bool SerializeGrid(const std::shared_ptr<Grid>& grid, nlohmann::json& json);
    bool SerializeImage(const std::shared_ptr<BGImage>& image, nlohmann::json& json);
    bool SerializeMatrix2D(const std::shared_ptr<Matrix2D>& matrix, nlohmann::json& json);
    // bool SerializeOverlay(const std::shared_ptr<Overlay>& overlay, nlohmann::json& json);
    bool SerializeScene(const std::shared_ptr<Scene>& scene, nlohmann::json& json);
    bool SerializeToken(const std::shared_ptr<Token>& token, nlohmann::json& json);

    nlohmann::json SerializeImages(const std::vector<std::shared_ptr<BGImage>>& images, bool selectedOnly=false);
    nlohmann::json SerializeTokens(const std::vector<std::shared_ptr<Token>>& tokens, bool selectedOnly=false);
    nlohmann::json SerializeScene(const std::shared_ptr<Scene>& scene);
    nlohmann::json SerializeScene(const std::shared_ptr<Scene>& scene, SerializeFlag flags);

    std::shared_ptr<Camera> DeserializeCamera(nlohmann::json& json);
    std::shared_ptr<Grid> DeserializeGrid(nlohmann::json& json);
    std::shared_ptr<BGImage> DeserializeImage(nlohmann::json& json);
    std::shared_ptr<Matrix2D> DeserializeMatrix2D(nlohmann::json& json);
    std::shared_ptr<Token> DeserializeToken(nlohmann::json& json);
    // std::shared_ptr<Overlay> DeserializeOverlay(nlohmann::json& json);
    void DeserializeScene(nlohmann::json& json, Scene& scene);
    std::shared_ptr<Scene> DeserializeScene(nlohmann::json& json);
    std::shared_ptr<Scene> DeserializeScene(const std::string& text);

private:
    std::shared_ptr<Resources> m_resources;
};
