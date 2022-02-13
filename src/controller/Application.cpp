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
    m_viewport = std::make_shared<Viewport>(1280, 720);

    glCreateRenderbuffers(1, &renderbuffer);
    glNamedRenderbufferStorage(renderbuffer, GL_RGBA, 1280, 720);
    glGenFramebuffers(1, &offScreenFrameBuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, offScreenFrameBuffer);
    glNamedFramebufferRenderbuffer(offScreenFrameBuffer, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderbuffer);

    // Render buffer is part of the viewport context
    // renderbuffer = std::make_shared<SharedRenderBuffer>(1280, 720);
    // renderbuffer->Add(m_viewport, BufferType::Write);

    // Resources must be loaded after the GL context is created by the window.
    LoadDefaultResources();

    m_uiWindow = std::make_shared<UIWindow>(1280, 720, m_resources, static_cast<std::shared_ptr<Window>>(m_viewport));
    // renderbuffer->Add(m_uiWindow, BufferType::Read);

    glGenFramebuffers(1, &tempFramebuffer);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, tempFramebuffer);
    glEnable(GL_RENDERBUFFER);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
    glNamedFramebufferRenderbuffer(tempFramebuffer, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderbuffer);


    controller = std::make_shared<Controller>(m_resources, m_viewport, m_uiWindow);
}

Application::~Application()
{
    m_viewport.reset();
    m_uiWindow.reset();
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
    return m_glfw_initialised && m_viewport->IsInitialised() && m_uiWindow->IsInitialised();
}

void Application::Exec()
{
    m_viewport->Use();
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    int width = m_viewport->Width();
    int height = m_viewport->Height();

    // Main loop
    while (!m_viewport->IsClosed())
    {
        glfwPollEvents();

        m_viewport->Use();
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);  // default framebuffer
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        m_viewport->Draw();
        glfwSwapBuffers(m_viewport->window);

        // renderbuffer->Use(m_viewport);
        // m_viewport->Draw();
        // renderbuffer->CopyToBuffer(m_uiWindow);

        // if (m_uiWindow)
        //     m_uiWindow->Draw();

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, offScreenFrameBuffer);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        m_viewport->Draw();

        m_uiWindow->Use();
        glBindFramebuffer(GL_READ_FRAMEBUFFER, tempFramebuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glViewport(0, 0, width, height);
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        glfwSwapBuffers(m_uiWindow->window);

        // Lazy hack to limit frame rate for now
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
