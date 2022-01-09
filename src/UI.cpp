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


void UIState::ClearSelection()
{
    for (Token* token : selectedTokens)
        token->isSelected = false;
    selectedTokens.clear();
}

void UIState::SelectToken(Token* token)
{
    selectedTokens.push_back(token);
    token->isSelected = true;
}


bool FilepathButton(const char* buttonName, const char* dialogName, const char* ext, std::string& path)
{
    if (ImGui::Button(buttonName))
        ImGuiFileDialog::Instance()->OpenDialog(dialogName, "Choose File", ext, "");

    bool success = false;
    if (ImGuiFileDialog::Instance()->Display(dialogName))
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


bool FileLine(std::string dialogName, std::string label, std::string& path)
{
    if (ImGui::InputText(label.c_str(), &path, ImGuiInputTextFlags_EnterReturnsTrue))
        return true;

    ImGui::SameLine();
    std::string buttonName = "Choose File##" + dialogName;
    if (FilepathButton(buttonName.c_str(), dialogName.c_str(), "Images{.png,.jpg,.jpeg}", path))
        return true;

    return false;
}


void DrawBackgroundOptions(BGImage* background, glm::vec4* bgColor)
{
    ImGui::ColorEdit3("Color##Background", (float*)bgColor);

    std::string imagePath = background->GetImage();
    if (FileLine("ChooseBGImage", "Image", imagePath))
        background->SetImage(imagePath);

    glm::vec2 bgPos = glm::vec2(background->GetPos());
    if (ImGui::DragFloat2("Position##Background", (float*)&bgPos))
        background->SetPos(glm::vec3(bgPos, 0));

    glm::vec2 bgSize = background->GetScale();
    if (ImGui::DragFloat2("Size##Background", (float*)&bgSize))
        background->SetScale(bgSize);

    float bgRot = background->GetRotation();
    if (ImGui::SliderFloat("Rotation##Background", &bgRot, 0, 360, "%.2f"))
        background->SetRotation(bgRot);
}

void DrawGridOptions(Grid* grid, UIState* uiState)
{
    float gridSize = grid->GetScale();
    if (ImGui::SliderFloat("Size##Grid", &gridSize, 0.1, 50, "%.3f", ImGuiSliderFlags_Logarithmic))
        grid->SetScale(gridSize);
    
    glm::vec3 gridColour = grid->GetColour();
    if (ImGui::ColorEdit3("Color##Grid", (float*)&gridColour))
        grid->SetColour(gridColour);
    
    ImGui::Checkbox("Snap to Grid", &uiState->snapToGrid);

}


void DrawTokenOptions(Token* token, Grid* grid, UIState* uiState)
{
    ImGui::InputText("Name", &token->name);

    std::string iconPath = token->GetIcon();
    if (FileLine("ChooseTokenIcon", "Icon", iconPath))
    {
        for (Token* t : uiState->selectedTokens)
            t->SetIcon(iconPath);
    }

    float iconSize = token->GetModel()->GetScalef();
    if (ImGui::SliderFloat("Size##Token", &iconSize, 0.1, 30, "%.3f", ImGuiSliderFlags_Logarithmic))
    {
        if (uiState->snapToGrid)
            iconSize = grid->SnapGridSize(iconSize);
        for (Token* t : uiState->selectedTokens)
            t->GetModel()->SetScalef(iconSize);
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


void DrawUI(Scene* scene, UIState* uiState)
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // UI window
    {
        ImGui::Begin("Mapmaker UI");

        if (ImGui::CollapsingHeader("Background"))
            DrawBackgroundOptions(&scene->background, &scene->bgColor);

        if (ImGui::CollapsingHeader("Grid"))
            DrawGridOptions(&scene->grid, uiState);

        ImGui::Text("Num selected tokens : %ld / %ld", uiState->selectedTokens.size(), scene->tokens.size());
        if (ImGui::CollapsingHeader("Token"))
        {
            if (uiState->selectedTokens.size() > 0)
                DrawTokenOptions(uiState->selectedTokens.back(), &scene->grid, uiState);

            if (ImGui::Button("Add Token"))
            {
                uiState->ClearSelection();
                scene->AddToken();
                uiState->SelectToken(&scene->tokens.back());
            }
        }

        // Spacer
        ImGui::Dummy(ImVec2(0.0f, 20.0f));

        // Save / Load
        std::string path;
        if (FilepathButton("Save", "saveDialog", ".json", path))
            scene->Save(path);
        
        ImGui::SameLine();

        if (FilepathButton("Load", "loadDialog", ".json", path))
            scene->Load(path);

        // Debug
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        ImGui::End();
    }

    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
