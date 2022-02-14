#include <stdio.h>
#include <chrono>
#include <thread>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Resources.h>
#include <glutil/Texture.h>
#include <model/Scene.h>
#include <view/UIWindow.h>
#include <view/Window.h>

#include <controller/Application.h>


const int GRID_SHADER = 1;

void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

Application::Application() : m_resources(std::make_shared<Resources>())
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return;

    m_glfw_initialised = true;

    // Initialises GL contexts
    m_hostWindow = std::make_shared<UIWindow>(1280, 720, m_resources);

    // Resources must be loaded after the GL context is created by the window.
    LoadDefaultResources();
    m_presentationWindow = std::make_shared<Viewport>(1280, 720, static_cast<std::shared_ptr<Window>>(m_hostWindow));
    controller = std::make_shared<Controller>(m_resources, m_presentationWindow, m_hostWindow);
}

Application::~Application()
{
    m_presentationWindow.reset();
    m_hostWindow.reset();
    controller.reset();
    m_resources.reset();
    if (m_glfw_initialised)
        glfwTerminate();
}

void Application::LoadDefaultResources()
{
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
    m_resources->CreateMesh(Resources::MeshType::Quad, vertices, indices);

    vertices = std::vector<Vertex>{
        {{-1.0f, -1.0f,  0.0f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}},
        {{ 1.0f, -1.0f,  0.0f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}},
        {{ 1.0f,  1.0f,  0.0f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}},
        {{-1.0f,  1.0f,  0.0f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}},
    };
    m_resources->CreateMesh(Resources::MeshType::Quad2, vertices, indices);

    m_resources->CreateShader(Resources::ShaderType::Grid, "resources/shaders/Grid.vs", "resources/shaders/Grid.fs");
    m_resources->CreateShader(Resources::ShaderType::ScreenRect, "resources/shaders/Grid.vs", "resources/shaders/Rect.fs");
    m_resources->CreateShader(Resources::ShaderType::Image, "resources/shaders/SimpleTexture.vs", "resources/shaders/SimpleTexture.fs");
    m_resources->CreateShader(Resources::ShaderType::Status, "resources/shaders/SimpleTexture.vs", "resources/shaders/Status.fs");
    m_resources->CreateShader(Resources::ShaderType::Token, "resources/shaders/SimpleTexture.vs", "resources/shaders/Token.fs");

    m_resources->CreateTexture(Resources::TextureType::Default, "resources/images/QuestionMark.jpg");
    m_resources->CreateTexture(Resources::TextureType::Status, "resources/images/StatusDot.png");
    m_resources->CreateTexture(Resources::TextureType::XStatus, "resources/images/XStatus.png");
}

bool Application::IsInitialised()
{
    return m_glfw_initialised && m_presentationWindow->IsInitialised() && m_hostWindow->IsInitialised();
}

void Application::Exec()
{
    m_hostWindow->Use();
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    // Main loop
    while (!m_hostWindow->IsClosed())
    {
        glfwPollEvents();
        controller->Render();
        // Lazy hack to limit frame rate for now
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
