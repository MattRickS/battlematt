#include <string>
#include <vector>

#include <glm/glm.hpp>

#include <BGImage.h>
#include <Camera.h>
#include <Grid.h>
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
    Shader imageShader;
    Shader tokenShader;

    Scene(Camera* camera, std::string bgPath):
        background(bgPath),
        grid(),
        camera(camera),
        imageShader("resources/shaders/SimpleTexture.vs", "resources/shaders/SimpleTexture.fs"),
        tokenShader("resources/shaders/SimpleTexture.vs", "resources/shaders/Token.fs")
    {}

    void AddToken(std::string iconPath, glm::vec3 pos=glm::vec3(0), float size=1.0f)
    {
        tokens.emplace_back(iconPath);
        tokens.back().SetPos(pos);
        tokens.back().SetSize(size);
    }

    void Draw()
    {
        glClearColor(bgColor.x * bgColor.w, bgColor.y * bgColor.w, bgColor.z * bgColor.w, bgColor.w);
        glClear(GL_COLOR_BUFFER_BIT);

        imageShader.use();
        background.Draw(imageShader);
        grid.Draw();

        tokenShader.use();
        for (Token& token : tokens)
            token.Draw(tokenShader);
    }
};