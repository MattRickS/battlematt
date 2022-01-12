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
#include <Primitives.h>
#include <Shader.h>
#include <Scene.h>
#include <Texture.h>
#include <Resources.h>
#include <UIWindow.h>
#include <Window.h>

#include <Application.h>


const int GRID_SHADER = 1;


Application::Application() : m_resources(std::make_shared<Resources>()), m_serializer(m_resources)
{
    InitGL();
    if (!IsInitialised())
        return;
    
    m_uiWindow = std::make_shared<UIWindow>();
    m_viewport = std::make_shared<Window>();
    m_inputManager = std::make_shared<InputManager>(m_scene, m_viewport, m_uiWindow);
}

void Application::LoadDefaultResources()
{
    resources->CreateMesh(
        Resources::MeshType::Quad,
        std::vector<Vertex>{
            {{-0.5f, -0.5f,  0.0f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}},
            {{ 0.5f, -0.5f,  0.0f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}},
            {{ 0.5f,  0.5f,  0.0f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}},
            {{-0.5f,  0.5f,  0.0f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}},
        }, std::vector<unsigned int> {
            0, 1, 2,
            2, 3, 0,
        }
    );
    resources->CreateMesh(
        Resources::MeshType::Quad2,
        std::vector<Vertex>{
            {{-1.0f, -1.0f,  0.0f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}},
            {{ 1.0f, -1.0f,  0.0f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}},
            {{ 1.0f,  1.0f,  0.0f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}},
            {{-1.0f,  1.0f,  0.0f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}},
        }, std::vector<unsigned int> {
            0, 1, 2,
            2, 3, 0,
        }
    );
    resources->CreateShader(Resources::ShaderType::Grid, "resources/shaders/Grid.vs", "resources/shaders/Grid.fs");
    resources->CreateShader(Resources::ShaderType::ScreenRect, "resources/shaders/Grid.vs", "resources/shaders/Rect.fs");
    resources->CreateShader(Resources::ShaderType::Image, "resources/shaders/SimpleTexture.vs", "resources/shaders/SimpleTexture.fs");
    resources->CreateShader(Resources::ShaderType::Token, "resources/shaders/SimpleTexture.vs", "resources/shaders/Token.fs");

    resources->CreateTexture(Resources::TextureType::Default, "resources/images/QuestionMark.jpg");
}

Application::~Application()
{
    if (m_glfw_initialised)
        glfwTerminate();
}

bool Application::IsInitialised()
{
    return m_glfw_initialised && m_glad_initialised && m_uiWindow->ID() && m_viewport->ID();
}

void Application::Draw()
{
    // TODO: Initialising here to prevent segault on init, this needs a cleaner setup
    CameraBuffer camBuffer = CameraBuffer();
    cameraBuffer = &camBuffer;

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_BLEND );

    // Main loop
    while (!m_viewport->ShouldClose())
    {
        glfwPollEvents();
        // TODO: Shouldn't have to update this every time
        //       Could possibly have a class BufferedCamera : Camera, UniformBuffer
        //       that updates its buffer when changed...?
        cameraBuffer->SetCamera(m_scene->camera);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Viewport would need the screenRect logic as well as Renderer
        m_viewport->Draw();
        // m_scene->Draw();
        // if (m_inputManager.dragSelectRect)
        //     m_inputManager.dragSelectRect->Draw();
        m_uiWindow->Draw(m_scene);

        // Lazy hack to limit frame rate for now
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

// =============================================================================
// Private

void Application::InitGL()
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return;

    m_glfw_initialised = true;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only

#if defined(__APPLE__)
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#endif

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }
    m_glad_initialised = true;
}

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
