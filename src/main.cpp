#include <iostream>
#include <stdio.h>
#include <chrono>
#include <thread>

#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <ImGuiFileDialog.h>

#include <Texture.h>
#include <Camera.h>
#include <Scene.h>


unsigned int windowWidth = 1280;
unsigned int windowHeight = 720;
bool firstMouse = true;
float lastMouseX, lastMouseY;
bool middleMouseHeld = false;
bool leftMouseHeld = false;
float deltaTime, lastFrame = 0.0f;
Scene* scene;
std::vector<Token*> selectedTokens;


// Orthographic operations (Matrices aren't working correctly if camera position is changed)
glm::vec2 ScreenToWorldPos(float x, float y)
{
    return glm::vec2(
        scene->camera->Position.x + (2 * x / windowWidth - 1) * (scene->camera->hAperture * scene->camera->Focal),
        scene->camera->Position.y + (2 * (1 - y / windowHeight) - 1) * (scene->camera->vAperture * scene->camera->Focal)
    );
}

glm::vec2 ScreenToWorldOffset(float x, float y)
{
    return glm::vec2(
        (x / windowWidth) * (scene->camera->hAperture * 2 * scene->camera->Focal),
        (y / windowHeight) * (scene->camera->vAperture * 2 * scene->camera->Focal)
    );
}

// =============================================================================
// Callbacks

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    scene->camera->SetAperture((float)width / (float)height);
    glViewport(0, 0, width, height);
    windowWidth = width;
    windowHeight = height;
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

    glm::vec2 worldPos = ScreenToWorldPos(xpos, ypos);
    for (Token& token : scene->tokens)
        token.isHighlighted = token.Contains(worldPos);

    if (middleMouseHeld)
        scene->camera->Pan(ScreenToWorldOffset(xoffset, yoffset));
    else if (leftMouseHeld && selectedTokens.size() > 0)
    {
        glm::vec2 offset = ScreenToWorldOffset(xoffset, yoffset);
        for (Token* token : selectedTokens)
            token->Move(offset);
    }
    // else
    //     scene->camera->ProcessMouseMovement(xoffset, yoffset);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse)
        return;

    if (button == GLFW_MOUSE_BUTTON_MIDDLE)
        middleMouseHeld = action == GLFW_PRESS;
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            for (Token* token : selectedTokens)
                token->isSelected = false;
            selectedTokens.clear();

            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            glm::vec2 worldPos = ScreenToWorldPos(xpos, ypos);
            for (Token& token : scene->tokens)
            {
                token.isSelected = token.Contains(worldPos);
                if (token.isSelected)
                    selectedTokens.push_back(&token);
            }
        }
        leftMouseHeld = action == GLFW_PRESS;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse)
    {
        std::cout << "Captured" << std::endl;
        return;
    }

    scene->camera->ProcessMouseScroll(yoffset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureKeyboard)
        return;

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)
        scene->camera->MovementSpeed *= 5;
    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE)
        scene->camera->MovementSpeed /= 5;
}

void setCallbacks(GLFWwindow* window)
{
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
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
        scene->camera->ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        scene->camera->ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        scene->camera->ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        scene->camera->ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        scene->camera->ProcessKeyboard(Camera_Movement::DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        scene->camera->ProcessKeyboard(Camera_Movement::UP, deltaTime);
}


namespace ImGui
{
    // ImGui::InputText() with std::string
    // Because text input needs dynamic resizing, we need to setup a callback to grow the capacity
    IMGUI_API bool  InputText(const char* label, std::string* str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
    IMGUI_API bool  InputTextMultiline(const char* label, std::string* str, const ImVec2& size = ImVec2(0, 0), ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
    IMGUI_API bool  InputTextWithHint(const char* label, const char* hint, std::string* str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
}

bool FileLine(std::string label, std::string& path)
{
    bool success = false;
    if (ImGui::InputText(label.c_str(), &path, ImGuiInputTextFlags_EnterReturnsTrue))
        return true;

    ImGui::SameLine();
    // TODO: Providing wildcard filter doesn't return correct path (uses * as ext)
    //       Ideally don't want to restrict filters so severely
    if (ImGui::Button("Open File Dialog")) 
        ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDialog", "Choose File", "Images{.png,.jpg,.jpeg}", path);

    if (ImGuiFileDialog::Instance()->Display("ChooseFileDialog"))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            path = ImGuiFileDialog::Instance()->GetFilePathName();
            success = true;
        }
        
        ImGuiFileDialog::Instance()->Close();
    }
    return success;
}


void DrawUI(float* bgSize)
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // UI window
    {
        ImGui::Begin("Mapmaker UI");

        ImGui::ColorEdit3("Background Color", (float*)&scene->bgColor);
        if (ImGui::SliderFloat("Background Size", bgSize, 1, 100))
            scene->background.SetScale(*bgSize);

        ImGui::Text("Num selected tokens : %ld", selectedTokens.size());
        if (selectedTokens.size() > 0)
        {
            Token* token = selectedTokens[0];
            ImGui::TextUnformatted(token->name.c_str());
            std::string iconPath = token->GetIcon();
            if (FileLine("Icon", iconPath))
                token->SetIcon(iconPath);
            float iconSize = token->GetSize();
            if (ImGui::SliderFloat("Size", &iconSize, 0, 1))
                token->SetSize(iconSize);
            ImGui::SliderFloat("Border Width", &token->borderWidth, 0, 1);
            ImGui::ColorEdit3("Border Colour", (float*)&token->borderColor);
        }
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        ImGui::End();
    }

    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


int main(int, char**)
{
    // Context guards initialise and teardown the GL and ImGui contexts
    InitGL glGuard = InitGL();
    if (!glGuard.IsInitialised())
        return 1;
    setCallbacks(glGuard.window);
    InitImgGui imguiGuard(glGuard.window, glGuard.glsl_version);

    // Scene
    stbi_set_flip_vertically_on_load(true);

    Camera camera = Camera(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f), true, 1.0f, windowWidth / windowHeight);
    Scene theScene = Scene(&camera, "resources/images/CaveMap.jpg");
    theScene.AddToken("resources/images/Dragon.jpeg", glm::vec3(0.3f, 0, 0), 0.1f);
    theScene.AddToken("resources/images/Dragon.jpeg", glm::vec3(0.3f, 0.3f, 0), 0.1f);
    theScene.AddToken("resources/images/Dragon.jpeg", glm::vec3(0, 0.3f, 0), 0.1f);

    scene = &theScene;

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_BLEND );

    // TODO: Initialise
    float bgSize;

    // Main loop
    while (!glfwWindowShouldClose(glGuard.window))
    {
        glfwPollEvents();

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        scene->Draw();
        DrawUI(&bgSize);

        glfwSwapBuffers(glGuard.window);

        // Lazy hack to limit frame rate for now
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return 0;
}
