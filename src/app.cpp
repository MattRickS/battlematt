#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <glutil/Buffers.h>
#include <glutil/Camera.h>
#include <glutil/Mesh.h>
#include <glutil/Shader.h>
#include <glutil/Texture.h>


int main(int numArgs, char* args[])
{
    if (!glfwInit())
        return -1;

    // Window
    GLFWwindow* window = glfwCreateWindow(640, 480, "A", NULL, NULL);
    if (window == NULL)
        return -1;
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        return -1;

    glViewport(0, 0, 640, 480);

    // Data
    auto vertices = std::vector<Vertex>{
        {{-0.5f, -0.5f,  0.0f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}},
        {{ 0.5f, -0.5f,  0.0f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.0f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}},
        {{-0.5f,  0.5f,  0.0f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}},
    };
    auto indices = std::vector<unsigned int> {
        0, 1, 2,
        2, 3, 0,
    };
    Mesh mesh = Mesh(vertices, indices);    

    Texture texture = Texture("resources/images/QuestionMark.jpg");
    texture.activate(GL_TEXTURE0);

    Shader shader = Shader("resources/shaders/SimpleTexture.vs", "resources/shaders/SimpleTexture.fs");
    shader.setInt("diffuse", 0);
    shader.setMat4("model", glm::mat4(1.0f));

    auto camera = std::make_shared<Camera>(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f), true, 10.0f);
    CameraBuffer buffer = CameraBuffer(camera);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        mesh.Draw(shader);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
