#pragma once
#include <memory>
#include <string>
#include <variant>

#include <glm/glm.hpp>


enum TokenProperty
{
    Token_Position,
    Token_Rotation,
    Token_Scale,
    Token_BorderWidth,
    Token_BorderColor,
    Token_Name,
    Token_Texture
};

enum ImageProperty
{
    Image_Position,
    Image_Rotation,
    Image_Scale,
    Image_Texture
};

typedef std::variant<float, glm::vec2, glm::vec4, std::string> TokenPropertyValue;
typedef std::variant<float, glm::vec2, std::string> ImagePropertyValue;
