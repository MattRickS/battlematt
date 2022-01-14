#include <iostream>
#include <stdio.h>
#include <chrono>
#include <thread>
#include <vector>

#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <ImGuiFileDialog.h>

#include <Buffers.h>
#include <Camera.h>
#include <Shader.h>
#include <Scene.h>
#include <Texture.h>
#include <Resources.h>
#include <UIWindow.h>
#include <Window.h>

#include <Application.h>


const int GRID_SHADER = 1;

void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

Application::Application() : m_resources(std::make_shared<Resources>()), m_serializer(m_resources)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return;

    m_glfw_initialised = true;

    m_scene = std::make_shared<Scene>(m_resources);
    m_uiWindow = std::make_shared<UIWindow>(200, 200, m_scene, m_resources);
    m_viewport = std::make_shared<Viewport>(200, 100, m_scene);
    m_controller = std::make_shared<Controller>(m_scene, m_viewport, m_uiWindow);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }
    m_glad_initialised = true;
    if (!IsInitialised())
        return;

    LoadDefaultResources();
}

Application::~Application()
{
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
    m_resources->CreateShader(Resources::ShaderType::Token, "resources/shaders/SimpleTexture.vs", "resources/shaders/Token.fs");

    m_resources->CreateTexture(Resources::TextureType::Default, "resources/images/QuestionMark.jpg");
}

bool Application::IsInitialised()
{
    return m_glfw_initialised && m_glad_initialised && m_uiWindow->IsInitialised() && m_viewport->IsInitialised();
}

void Application::Exec()
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_BLEND );

    // Main loop
    while (!m_viewport->IsClosing())
    {
        glfwPollEvents();

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Viewport would need the screenRect logic as well as Renderer
        m_viewport->Draw();
        // m_scene->Draw();
        // if (m_inputManager.dragSelectRect)
        //     m_controller.dragSelectRect->Draw();
        m_uiWindow->Draw(m_controller->uiState);

        // Lazy hack to limit frame rate for now
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

// =============================================================================
// Private

void Application::Save(std::string path)
{
    std::cerr << "Saving to " << path << std::endl;
    std::ofstream myfile (path);
    if (myfile.is_open())
    {
        myfile << m_serializer.SerializeScene(m_scene);
        myfile.close();
    }
    else
        std::cerr << "Unable to open file" << std::endl;
}

void Application::Load(std::string path)
{
    std::cerr << "Loading Scene from " << path << std::endl;
    nlohmann::json j;
    std::ifstream myfile (path);
    if (myfile.is_open())
    {
        myfile >> j;
        myfile.close();
        m_scene = m_serializer.DeserializeScene(j);
    }
    else
        std::cerr << "Unable to open file" << std::endl;
}
