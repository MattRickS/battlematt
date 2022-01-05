#include <vector>

#include <glm/glm.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <ImGuiFileDialog.h>

#include <Scene.h>
#include <Token.h>
#include <UI.h>


namespace ImGui
{
    // ImGui::InputText() with std::string
    // Because text input needs dynamic resizing, we need to setup a callback to grow the capacity
    IMGUI_API bool  InputText(const char* label, std::string* str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
    IMGUI_API bool  InputTextMultiline(const char* label, std::string* str, const ImVec2& size = ImVec2(0, 0), ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
    IMGUI_API bool  InputTextWithHint(const char* label, const char* hint, std::string* str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
}


ImGuiContextGuard::ImGuiContextGuard(GLFWwindow* window, const char* glsl_version)
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

ImGuiContextGuard::~ImGuiContextGuard()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
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


void DrawUI(Scene* scene, UIState* uiState)
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // UI window
    {
        ImGui::Begin("Mapmaker UI");

        ImGui::ColorEdit3("Background Color", (float*)&scene->bgColor);
        float bgSize = scene->background.GetScale();
        if (ImGui::SliderFloat("Background Size", &bgSize, 1, 100, "%.3f", ImGuiSliderFlags_Logarithmic))
            scene->background.SetScale(bgSize);
        
        std::string imagePath = scene->background.GetImage();
        if (FileLine("Background Image", imagePath))
            scene->background.SetImage(imagePath);
      
        float gridSize = scene->grid.GetScale();
        if (ImGui::SliderFloat("Grid Size", &gridSize, 0.1, 50, "%.3f", ImGuiSliderFlags_Logarithmic))
            scene->grid.SetScale(gridSize);
        
        glm::vec3 gridColour = scene->grid.GetColour();
        if (ImGui::ColorEdit3("Grid Color", (float*)&gridColour))
            scene->grid.SetColour(gridColour);
        
        ImGui::Checkbox("Snap to Grid", &uiState->snapToGrid);

        ImGui::Text("Num selected tokens : %ld", uiState->selectedTokens.size());
        if (uiState->selectedTokens.size() > 0)
        {
            Token* token = uiState->selectedTokens.back();
            ImGui::TextUnformatted(token->name.c_str());

            std::string iconPath = token->GetIcon();
            if (FileLine("Icon", iconPath))
            {
                for (Token* t : uiState->selectedTokens)
                    t->SetIcon(iconPath);
            }

            float iconSize = token->GetSize();
            if (ImGui::SliderFloat("Size", &iconSize, 0.1, 30, "%.3f", ImGuiSliderFlags_Logarithmic))
            {
                if (uiState->snapToGrid)
                    iconSize = scene->grid.SnapGridSize(iconSize);
                for (Token* t : uiState->selectedTokens)
                    t->SetSize(iconSize);
            }

            if (ImGui::SliderFloat("Border Width", &token->borderWidth, 0, 1))
            {
                for (Token* t : uiState->selectedTokens)
                    t->borderWidth = token->borderWidth;
            }
            if (ImGui::ColorEdit3("Border Colour", (float*)&token->borderColor))
            {
                for (Token* t : uiState->selectedTokens)
                    t->borderColor = token->borderColor;
            }
        }

        if (ImGui::Button("Add Token"))
            scene->AddToken();

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        ImGui::End();
    }

    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}