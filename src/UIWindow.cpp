#include <vector>

#include <glm/glm.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <ImGuiFileDialog.h>

#include <Matrix2D.h>
#include <Shape2D.h>
#include <Scene.h>
#include <Token.h>
#include <UIState.h>

#include <UIWindow.h>


namespace ImGui
{
    // ImGui::InputText() with std::string
    // Because text input needs dynamic resizing, we need to setup a callback to grow the capacity
    IMGUI_API bool  InputText(const char* label, std::string* str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
    IMGUI_API bool  InputTextMultiline(const char* label, std::string* str, const ImVec2& size = ImVec2(0, 0), ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
    IMGUI_API bool  InputTextWithHint(const char* label, const char* hint, std::string* str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
}


UIWindow::UIWindow(unsigned int width, unsigned int height, std::shared_ptr<Scene> scene, std::shared_ptr<Resources> resources) :
    Window(width, height, "UI"), m_scene(scene), m_resources(resources)
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
    ImGui_ImplOpenGL3_Init("#version 330");

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

UIWindow::~UIWindow()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
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

void UIWindow::DrawMatrix2DOptions(std::string suffixID, Matrix2D* matrix2D)
{
    glm::vec2 pos = matrix2D->GetPos();
    if (ImGui::DragFloat2(("Position##" + suffixID).c_str(), (float*)&pos))
        matrix2D->SetPos(pos);

    glm::vec2 scale = matrix2D->GetScale();
    if (ImGui::DragFloat2(("Size##" + suffixID).c_str(), (float*)&scale))
        matrix2D->SetScale(scale);

    float rotation = matrix2D->GetRotation();
    if (ImGui::SliderFloat(("Rotation##" + suffixID).c_str(), &rotation, 0, 360, "%.2f"))
        matrix2D->SetRotation(rotation);
}

// Draw a single set of UI inputs and copy all changes to each Shape2D
void UIWindow::DrawShape2DOptions(std::string suffixID, std::vector<std::shared_ptr<Shape2D>>& shapes, std::shared_ptr<Grid> grid, bool snapToGrid, bool singleScale)
{
    Matrix2D* matrix2D = shapes[0]->GetModel();

    glm::vec2 pos = matrix2D->GetPos();
    if (ImGui::DragFloat2(("Position##" + suffixID).c_str(), (float*)&pos))
    {
        for (std::shared_ptr<Shape2D> shape : shapes)
        {
            // TODO: Get relaative offset and apply offset to all shapes rather than move everything on top of each other
            if (snapToGrid)
                pos = grid->ShapeSnapPosition(shape, pos);
            shape->GetModel()->SetPos(pos);
        }
    }

    glm::vec2 scale = matrix2D->GetScale();
    if (singleScale && ImGui::SliderFloat(("Size##1" + suffixID).c_str(), &scale.x, 0, 100, "%.3f", ImGuiSliderFlags_Logarithmic))
        for (std::shared_ptr<Shape2D> shape : shapes)
        {
            if (snapToGrid)
                scale = glm::vec2(grid->SnapGridSize(scale.x));
            shape->GetModel()->SetScalef(scale.x);
        }
    else if (!singleScale && ImGui::DragFloat2(("Size##2" + suffixID).c_str(), (float*)&scale, 0.5f))
    {
        for (std::shared_ptr<Shape2D> shape : shapes)
        {
            if (snapToGrid)
                scale = glm::vec2(grid->SnapGridSize(scale.x), grid->SnapGridSize(scale.y));
            shape->GetModel()->SetScale(scale);
        }

    }

    float rotation = matrix2D->GetRotation();
    if (ImGui::SliderFloat(("Rotation##" + suffixID).c_str(), &rotation, 0, 360, "%.2f"))
        for (std::shared_ptr<Shape2D> shape : shapes)
            shape->GetModel()->SetRotation(rotation);
}

void UIWindow::DrawBackgroundOptions(std::shared_ptr<BGImage> background, glm::vec4* bgColor)
{
    ImGui::ColorEdit3("Color##Background", (float*)bgColor);

    std::string imagePath = background->GetImage()->filename;
    if (FileLine("ChooseBGImage", "Image", imagePath))
        background->SetImage(m_resources->GetTexture(imagePath));

    DrawMatrix2DOptions("Background", background->GetModel());
}

void UIWindow::DrawGridOptions(std::shared_ptr<Grid> grid, std::shared_ptr<UIState> uiState)
{
    float gridSize = grid->GetScale();
    if (ImGui::SliderFloat("Size##Grid", &gridSize, 0.1, 50, "%.3f", ImGuiSliderFlags_Logarithmic))
        grid->SetScale(gridSize);
    
    glm::vec3 gridColour = grid->GetColour();
    if (ImGui::ColorEdit3("Color##Grid", (float*)&gridColour))
        grid->SetColour(gridColour);
    
    ImGui::Checkbox("Snap to Grid", &uiState->snapToGrid);

}

void UIWindow::DrawTokenOptions(std::vector<std::shared_ptr<Token>> tokens, std::shared_ptr<Grid> grid, bool snapToGrid)
{
    std::shared_ptr<Token> token = tokens[0];

    ImGui::InputText("Name", &token->name);

    std::string iconPath = token->GetIcon()->filename;
    if (FileLine("ChooseTokenIcon", "Icon", iconPath))
    {
        for (std::shared_ptr<Token> t : tokens)
            t->SetIcon(m_resources->GetTexture(iconPath));
    }

    if (ImGui::SliderFloat("Border Width", &token->borderWidth, 0, 1))
    {
        for (std::shared_ptr<Token> t : tokens)
            t->borderWidth = token->borderWidth;
    }
    if (ImGui::ColorEdit3("Border Colour", (float*)&token->borderColor))
    {
        for (std::shared_ptr<Token> t : tokens)
            t->borderColor = token->borderColor;
    }

    std::vector<std::shared_ptr<Shape2D>> shapes(tokens.size());
    std::transform(tokens.begin(), tokens.end(), shapes.begin(), [](std::shared_ptr<Token> t){ return static_cast<std::shared_ptr<Shape2D>>(t); });
    DrawShape2DOptions("Token", shapes, grid, snapToGrid, true);
}

void UIWindow::Draw(std::shared_ptr<UIState> uiState)
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // UI window
    {
        ImGui::Begin("Mapmaker UI");

        if (ImGui::CollapsingHeader("Background"))
        {
            if (m_scene->backgrounds.size() > 0)
                DrawBackgroundOptions(m_scene->backgrounds.back(), &m_scene->bgColor);
        }

        if (ImGui::CollapsingHeader("Grid"))
            DrawGridOptions(m_scene->grid, uiState);

        ImGui::Text("Num selected tokens : %ld / %ld", uiState->selectedTokens.size(), m_scene->tokens.size());
        if (ImGui::CollapsingHeader("Token"))
        {
            if (uiState->selectedTokens.size() > 0)
                DrawTokenOptions(uiState->selectedTokens, m_scene->grid, uiState->snapToGrid);

            if (ImGui::Button("Add Token"))
                addTokenClicked.emit();
        }

        // Spacer
        ImGui::Dummy(ImVec2(0.0f, 20.0f));

        // Save / Load
        std::string path;
        if (FilepathButton("Save", "saveDialog", ".json", path))
            saveClicked.emit(path);
        
        ImGui::SameLine();

        if (FilepathButton("Load", "loadDialog", ".json", path))
            loadClicked.emit(path);

        // Debug
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        ImGui::End();
    }

    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
