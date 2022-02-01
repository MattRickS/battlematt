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

    // Window 1
    GLFWwindow* window1 = glfwCreateWindow(640, 480, "A", NULL, NULL);
    if (window1 == NULL)
        return -1;
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only

    glfwMakeContextCurrent(window1);
    glfwSwapInterval(1); // Enable vsync

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        return -1;

    auto camera = std::make_shared<Camera>(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f), true, 10.0f);

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
    Mesh mesh1 = Mesh(vertices, indices);    
    Texture texture1 = Texture("resources/images/QuestionMark.jpg");
    Shader shader1 = Shader("resources/shaders/SimpleTexture.vs", "resources/shaders/SimpleTexture.fs");
    CameraBuffer buffer1 = CameraBuffer(camera);

    // Window2
    GLFWwindow* window2 = glfwCreateWindow(640, 480, "B", NULL, window1);
    if (window2 == NULL)
        return -1;
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only

    glfwMakeContextCurrent(window2);
    glfwSwapInterval(1); // Enable vsync

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        return -1;
    
    Texture texture2 = Texture("resources/images/QuestionMark.jpg");
    Shader shader2 = Shader("resources/shaders/SimpleTexture.vs", "resources/shaders/SimpleTexture.fs");
    CameraBuffer buffer2 = CameraBuffer(camera);
    mesh1.GenerateVAO();

    while (!glfwWindowShouldClose(window1) && !glfwWindowShouldClose(window2))
    {
        glfwPollEvents();

        glfwMakeContextCurrent(window1);
        glViewport(0, 0, 640, 480);
        glClear(GL_COLOR_BUFFER_BIT);
        texture1.activate(GL_TEXTURE0);
        shader1.use();
        shader1.setInt("diffuse", 0);
        shader1.setMat4("model", glm::mat4(1.0f));
        mesh1.Draw(shader1);
        glfwSwapBuffers(window1);

        glfwMakeContextCurrent(window2);
        glViewport(0, 0, 640, 480);
        glClear(GL_COLOR_BUFFER_BIT);
        texture2.activate(GL_TEXTURE0);
        shader2.use();
        shader2.setInt("diffuse", 0);
        shader2.setMat4("model", glm::mat4(1.0f));
        mesh1.Draw(shader2);
        glfwSwapBuffers(window2);
    }

    glfwTerminate();
    return 0;
}
