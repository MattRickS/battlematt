#include <iostream>
#include <stdio.h>

#include <glad/glad.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

#include <Texture.hpp>
#include <Camera.hpp>
#include <Primitives.hpp>


unsigned int windowWidth = 1280;
unsigned int windowHeight = 720;
bool firstMouse = true;
float lastMouseX, lastMouseY;
float deltaTime, lastFrame = 0.0f;
Camera mainCamera = Camera();

// =============================================================================
// Callbacks

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // TODO: Reposition camera
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse)
        return;

    if (firstMouse)
    {
        lastMouseX = xpos;
        lastMouseX = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastMouseX;
    float yoffset = lastMouseY - ypos; // reversed since y-coordinates range from bottom to top
    lastMouseX = xpos;
    lastMouseY = ypos;

    mainCamera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse)
        return;

    mainCamera.ProcessMouseScroll(yoffset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureKeyboard)
        return;

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)
        mainCamera.MovementSpeed *= 5;
    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE)
        mainCamera.MovementSpeed /= 5;
}

void setCallbacks(GLFWwindow* window)
{
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
}

// =============================================================================
// Context Guards

class InitGL
{
public:
    GLFWwindow* window;
    const char* glsl_version = "#version 330";
    
    static void glfw_error_callback(int error, const char* description)
    {
        fprintf(stderr, "Glfw Error %d: %s\n", error, description);
    }

    InitGL()
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

        // Create window with graphics context
        window = glfwCreateWindow(windowWidth, windowHeight, "Main Window", NULL, NULL);
        if (window == NULL)
            return;

        glfwMakeContextCurrent(window);
        glfwSwapInterval(1); // Enable vsync


        // Initialize GLAD
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return;
        }
        m_glad_initialised = true;
    }

    ~InitGL()
    {
        if (window)
            glfwDestroyWindow(window);
        if (m_glfw_initialised)
            glfwTerminate();
    }

    bool IsInitialised()
    {
        return window && m_glfw_initialised && m_glad_initialised;
    }

private:
    bool m_glfw_initialised = false;
    bool m_glad_initialised = false;
};

class InitImgGui
{
public:
    InitImgGui(GLFWwindow* window, const char* glsl_version)
    {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);

        // Load Fonts
        // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
        // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
        // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
        // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
        // - Read 'docs/FONTS.md' for more instructions and details.
        // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
        //io.Fonts->AddFontDefault();
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
        //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
        //IM_ASSERT(font != NULL);
    }

    ~InitImgGui()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
};


// =============================================================================


void processInput(GLFWwindow *window)
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureKeyboard)
        return;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        mainCamera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        mainCamera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        mainCamera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        mainCamera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        mainCamera.ProcessKeyboard(Camera_Movement::DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        mainCamera.ProcessKeyboard(Camera_Movement::UP, deltaTime);
}


int main(int, char**)
{
    // Context guards initialise and teardown the GL and ImGui contexts
    InitGL glGuard = InitGL();
    if (!glGuard.IsInitialised())
        return 1;
    setCallbacks(glGuard.window);
    InitImgGui imguiGuard(glGuard.window, glGuard.glsl_version);

    // Our state
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ImVec4 cube_color = ImVec4(1.0f, 0.5f, 0.2f, 1.0f);

    // Scene
    stbi_set_flip_vertically_on_load(true);
    Texture map = Texture("resources/images/CaveMap.jpg");
    Shader basicShader = Shader("resources/shaders/Simple.vs", "resources/shaders/Simple.fs");
    glBindTexture(GL_TEXTURE_2D, map.ID);
    basicShader.use();
    basicShader.setInt("diffuse", 0);
    // basicShader.setFloat4("color", cube_color.x, cube_color.y, cube_color.z, cube_color.w);
    Quad quad;
    glm::mat4 view;
    glm::mat4 projection = glm::perspective(glm::radians(mainCamera.Zoom), (float)windowWidth/(float)windowHeight, 0.1f, 100.0f);
    glm::mat4 model = glm::mat4(1.0f);

    // Main loop
    while (!glfwWindowShouldClose(glGuard.window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(glGuard.window);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            if (ImGui::ColorEdit3("Cube Color", (float*)&cube_color))
                basicShader.setFloat4("color", cube_color.x, cube_color.y, cube_color.z, cube_color.w);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw Scene
        view = mainCamera.GetViewMatrix();
        basicShader.use();
        basicShader.setMat4("model", model);
        basicShader.setMat4("view", view);
        basicShader.setMat4("projection", projection);
        map.activate(GL_TEXTURE0);
        quad.Draw(basicShader);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(glGuard.window);
    }

    return 0;
}
