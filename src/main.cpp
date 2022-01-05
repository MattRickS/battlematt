#include <glad/glad.h>
#include <glm/glm.hpp>

#include <Application.h>
#include <Camera.h>
#include <Scene.h>
#include <UI.h>


int main(int, char**)
{
    Application app = Application();
    if (!app.IsInitialised())
        return 1;
    ImGuiContextGuard imguiGuard(app.Window(), app.glsl_version);
    stbi_set_flip_vertically_on_load(true);

    glm::vec2 windowDimensions = app.WindowDimensions();
    Camera camera = Camera(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f), true, 10.0f, windowDimensions.x / windowDimensions.y);
    Scene scene = Scene(&camera);
    scene.AddToken("resources/images/Dragon.jpeg", glm::vec3(0.5f, 0.5, 0));
    scene.AddToken("resources/images/Dragon.jpeg", glm::vec3(1.5f, 1.5f, 0));

    app.scene = &scene;
    app.Draw();

    return 0;
}
