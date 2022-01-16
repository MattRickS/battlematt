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


UIWindow::UIWindow(unsigned int width, unsigned int height, std::shared_ptr<Resources> resources, std::shared_ptr<Window> share) :
    Window(width, height, "UI", share), m_resources(resources)
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

void UIWindow::SetScene(std::shared_ptr<Scene> scene) { m_scene = scene; }

void UIWindow::Prompt(int promptType, std::string msg)
{
    m_promptMsg = msg;
    m_promptType = promptType;
    Focus();
}

bool UIWindow::HasPrompt()
{
    return m_promptType != 0;
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

void UIWindow::DrawMatrix2DOptions(std::string suffixID, Matrix2D* matrix2D, bool lockScaleRatio)
{
    glm::vec2 pos = matrix2D->GetPos();
    if (ImGui::DragFloat2(("Position##" + suffixID).c_str(), (float*)&pos))
        matrix2D->SetPos(pos);

    // TODO: Reset size option to match image ratio
    if (lockScaleRatio)
    {
        float size = matrix2D->GetScalef();
        if (ImGui::SliderFloat(("Scale##" + suffixID).c_str(), &size, 0.1, 1000, "%.2f", ImGuiSliderFlags_Logarithmic))
            matrix2D->SetScale(matrix2D->GetScale() * (size / matrix2D->GetScalef()));
    }
    else
    {
        glm::vec2 scale = matrix2D->GetScale();
        if (ImGui::DragFloat2(("Size##" + suffixID).c_str(), (float*)&scale))
            matrix2D->SetScale(scale);
    }

    float rotation = matrix2D->GetRotation();
    if (ImGui::SliderFloat(("Rotation##" + suffixID).c_str(), &rotation, 0, 360, "%.2f"))
        matrix2D->SetRotation(rotation);
}

// Draw a single set of UI inputs and copy all changes to each Shape2D
void UIWindow::DrawShape2DOptions(std::string suffixID, std::shared_ptr<Shape2D> shape, std::shared_ptr<Grid> grid, bool snapToGrid, bool singleScale)
{
    Matrix2D* matrix2D = shape->GetModel();

    glm::vec2 pos = matrix2D->GetPos();
    if (ImGui::DragFloat2(("Position##" + suffixID).c_str(), (float*)&pos))
    {
        // TODO: Get relaative offset and apply offset to all shapes rather than move everything on top of each other
        if (snapToGrid)
            pos = grid->ShapeSnapPosition(shape, pos);
        shape->GetModel()->SetPos(pos);
    }

    glm::vec2 scale = matrix2D->GetScale();
    if (singleScale && ImGui::SliderFloat(("Size##1" + suffixID).c_str(), &scale.x, 0, 100, "%.3f", ImGuiSliderFlags_Logarithmic))
    {
        if (snapToGrid)
            scale = glm::vec2(grid->SnapGridSize(scale.x));
        shape->GetModel()->SetScalef(scale.x);
    }
    else if (!singleScale && ImGui::DragFloat2(("Size##2" + suffixID).c_str(), (float*)&scale, 0.5f))
    {
        if (snapToGrid)
            scale = glm::vec2(grid->SnapGridSize(scale.x), grid->SnapGridSize(scale.y));
        shape->GetModel()->SetScale(scale);
    }

    float rotation = matrix2D->GetRotation();
    if (ImGui::SliderFloat(("Rotation##" + suffixID).c_str(), &rotation, 0, 360, "%.2f"))
        shape->GetModel()->SetRotation(rotation);
}

void UIWindow::DrawImageOptions(std::shared_ptr<BGImage> image)
{
    std::string imagePath = image->GetImage()->filename;
    if (FileLine("ChooseBGImage", "Image", imagePath))
        image->SetImage(m_resources->GetTexture(imagePath));

    ImGui::Checkbox("Lock Size Ratio", &image->lockRatio);
    DrawMatrix2DOptions("Background", image->GetModel(), image->lockRatio);
}

void UIWindow::DrawGridOptions(std::shared_ptr<Grid> grid)
{
    float gridSize = grid->GetScale();
    if (ImGui::SliderFloat("Size##Grid", &gridSize, 0.1, 50, "%.3f", ImGuiSliderFlags_Logarithmic))
        grid->SetScale(gridSize);
    
    glm::vec3 gridColour = grid->GetColour();
    if (ImGui::ColorEdit3("Color##Grid", (float*)&gridColour))
        grid->SetColour(gridColour);
    
    ImGui::Checkbox("Snap to Grid", &uiState->snapToGrid);

}

void UIWindow::DrawTokenOptions(std::shared_ptr<Token> token, std::shared_ptr<Grid> grid, bool snapToGrid)
{
    // TODO: How to apply to all selected? Pass in the selected? Calculate selected here?
    //       Emit a signal for each property?
    //       Generate an action, emit a signal, and let the Controller expand that action to be a list of objects?
    ImGui::InputText("Name", &token->name);

    std::string iconPath = token->GetIcon()->filename;
    if (FileLine("ChooseTokenIcon", "Icon", iconPath))
        token->SetIcon(m_resources->GetTexture(iconPath));

    if (ImGui::SliderFloat("Border Width", &token->borderWidth, 0, 1))
        token->borderWidth = token->borderWidth;

    if (ImGui::ColorEdit3("Border Colour", (float*)&token->borderColor))
        token->borderColor = token->borderColor;

    DrawShape2DOptions("Token", static_cast<std::shared_ptr<Shape2D>>(token), grid, snapToGrid, true);
}

void UIWindow::Draw()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    static ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);

    // UI window
    static bool p_open = NULL;
    {
        ImGui::Begin("Mapmaker UI", &p_open, flags);

        ImGui::ColorEdit3("Background Color", (float*)&m_scene->bgColor);

        if (ImGui::CollapsingHeader("Images"))
        {
            static unsigned int selected_image_idx = 0;
            if (ImGui::BeginListBox("Images##List"))
            {
                for (unsigned int i = 0; i < m_scene->images.size(); i++)
                {
                    const bool is_selected = (selected_image_idx == i);
                    if (ImGui::Selectable((m_scene->images[i]->GetImage()->Name() + "##Item" + std::to_string(i)).c_str(), is_selected))
                        selected_image_idx = i;

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndListBox();
            }

            if (ImGui::Button("Add Image"))
                addImageClicked.emit();

            if (selected_image_idx < m_scene->images.size())
                DrawImageOptions(m_scene->images[selected_image_idx]);
        }

        if (ImGui::CollapsingHeader("Grid"))
            DrawGridOptions(m_scene->grid);

        if (ImGui::CollapsingHeader("Token"))
        {
            std::shared_ptr<Token> lastSelectedToken = nullptr;
            if (ImGui::BeginListBox("Tokens##List"))
            {
                int i = 0;
                for (std::shared_ptr<Token>& token : m_scene->tokens)
                {
                    if (ImGui::Selectable((token->name + "##Item" + std::to_string(i++)).c_str(), token->isSelected))
                        tokenSelectionChanged.emit(token, false);
                    if (token->isSelected)
                        lastSelectedToken = token;
                }
                ImGui::EndListBox();
            }

            if (ImGui::Button("Add Token"))
                addTokenClicked.emit();

            if (lastSelectedToken)
                DrawTokenOptions(lastSelectedToken, m_scene->grid, uiState->snapToGrid);
        }

        // Spacer
        ImGui::Dummy(ImVec2(0.0f, 20.0f));

        // Save / Load
        std::string path;
        if (!m_scene->sourceFile.empty())
        {
            ImGui::TextUnformatted(("Current Scene: " + m_scene->sourceFile).c_str());
            if (ImGui::Button("Save"))
                saveClicked.emit(m_scene->sourceFile);
        }

        ImGui::SameLine();
        if (FilepathButton("Save As", "saveDialog", ".json", path))
            saveClicked.emit(path);
        
        ImGui::SameLine();
        if (FilepathButton("Load", "loadDialog", ".json", path))
            loadClicked.emit(path, mergeLoad);

        ImGui::SameLine();
        ImGui::Checkbox("Merge into current scene", &mergeLoad);

        // Debug
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        ImGui::End();
    }

    // Prompt Dialog
    if (HasPrompt())
        ImGui::OpenPopup("PromptModal");
    // Always center this window when appearing
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("PromptModal", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextUnformatted(m_promptMsg.c_str());
        ImGui::Separator();

        if (ImGui::Button("OK", ImVec2(120, 0)))
            RespondToPrompt(true);
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
            RespondToPrompt(false);
        ImGui::EndPopup();
    }

    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UIWindow::RespondToPrompt(bool response)
{
    // Reset internal prompt type before triggering the callback in case another prompt is sent
    int promptType = m_promptType;
    ImGui::CloseCurrentPopup();
    m_promptType = 0;
    m_promptMsg = "";
    promptResponse.emit(promptType, response);
}
