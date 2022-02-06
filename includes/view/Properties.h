#pragma once
#include <memory>
#include <string>
#include <variant>

#include <glm/glm.hpp>

#include <model/Token.h>


enum TokenProperty
{
    Token_Position,
    Token_Rotation,
    Token_Scale,
    Token_BorderWidth,
    Token_BorderColor,
    Token_Name,
    Token_Texture,
    Token_Statuses,
    Token_XStatus,
    Token_Opacity
};

enum ImageProperty
{
    Image_Position,
    Image_Rotation,
    Image_Scale,
    Image_Texture,
    Image_LockRatio
};

enum GridProperty
{
    Grid_Snap,
    Grid_Scale,
    Grid_Color
};

enum CameraProperty
{
    Camera_Name
};

typedef std::variant<float, bool, TokenStatuses, glm::vec2, glm::vec4, std::string> TokenPropertyValue;
typedef std::variant<float, glm::vec2, bool, std::string> ImagePropertyValue;
typedef std::variant<float, glm::vec3, bool> GridPropertyValue;
typedef std::variant<std::string> CameraPropertyValue;
