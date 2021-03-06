#include <vector>

#include <glm/glm.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <ImGuiFileDialog.h>

#include <glutil/Matrix2D.h>
#include <model/Shape2D.h>
#include <model/Scene.h>
#include <model/Token.h>

#include <view/UIWindow.h>


namespace ImGui
{
    // ImGui::InputText() with std::string
    // Because text input needs dynamic resizing, we need to setup a callback to grow the capacity
    IMGUI_API bool  InputText(const char* label, std::string* str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
    IMGUI_API bool  InputTextMultiline(const char* label, std::string* str, const ImVec2& size = ImVec2(0, 0), ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
    IMGUI_API bool  InputTextWithHint(const char* label, const char* hint, std::string* str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
}


const ImVec4 SELECT_COLOR = ImVec4(0.3f, 0.3f, 1.0f, 1.0f);


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

void UIWindow::SetDisplayPropertiesToken(const std::shared_ptr<Token>& token)
{
    m_displayPropertiesToken = token;
}

void UIWindow::SetDisplayPropertiesImage(const std::shared_ptr<BGImage>& image)
{
    m_displayPropertiesImage = image;
}

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

void UIWindow::DrawImageOptions(const std::shared_ptr<BGImage>& image)
{
    std::string imagePath = image->GetImage()->filename;
    if (FileLine("ChooseBGImage", "Image", imagePath))
        imagePropertyChanged.emit(image, Image_Texture, ImagePropertyValue(imagePath));

    bool lockRatio = image->GetLockRatio();
    if (ImGui::Checkbox("Lock Size Ratio", &lockRatio))
        imagePropertyChanged.emit(image, Image_LockRatio, ImagePropertyValue(lockRatio));

    const std::shared_ptr<Matrix2D>& matrix2D = image->GetModel();

    glm::vec2 pos = matrix2D->GetPos();
    if (ImGui::DragFloat2("Position##Image", (float*)&pos))
        imagePropertyChanged.emit(image, Image_Position, ImagePropertyValue(pos));

    float scale = matrix2D->GetScale().x;
    if (ImGui::SliderFloat("Size##Image", &scale, 0, 100, "%.3f", ImGuiSliderFlags_Logarithmic))
        imagePropertyChanged.emit(image, Image_Scale, ImagePropertyValue(glm::vec2(scale, scale)));

    float rotation = matrix2D->GetRotation();
    if (ImGui::SliderFloat("Rotation##Image", &rotation, 0, 360, "%.2f"))
        imagePropertyChanged.emit(image, Image_Rotation, ImagePropertyValue(rotation));
}

void UIWindow::DrawGridOptions(const std::shared_ptr<Grid>& grid)
{
    float gridSize = grid->GetScale();
    if (ImGui::SliderFloat("Size##Grid", &gridSize, 0.1, 50, "%.3f", ImGuiSliderFlags_Logarithmic))
        gridPropertyChanged.emit(grid, Grid_Scale, GridPropertyValue(gridSize));
    
    glm::vec3 gridColour = grid->GetColour();
    if (ImGui::ColorEdit3("Color##Grid", (float*)&gridColour))
        gridPropertyChanged.emit(grid, Grid_Color, GridPropertyValue(gridColour));
    
    bool snapToGrid = grid->GetSnapEnabled();
    if (ImGui::Checkbox("Snap to Grid", &snapToGrid))
        gridPropertyChanged.emit(grid, Grid_Snap, GridPropertyValue(snapToGrid));
}

void UIWindow::DrawTokenOptions(const std::shared_ptr<Token>& token)
{
    std::string name = token->GetName();
    if (ImGui::InputText("Name", &name, ImGuiInputTextFlags_EnterReturnsTrue))
        tokenPropertyChanged.emit(token, Token_Name, TokenPropertyValue(name));

    std::string iconPath = token->GetIcon()->filename;
    if (FileLine("ChooseTokenIcon", "Icon", iconPath))
        tokenPropertyChanged.emit(token, Token_Texture, TokenPropertyValue(iconPath));

    float borderWidth = token->GetBorderWidth();
    if (ImGui::SliderFloat("Border Width", &borderWidth, 0, 1))
        tokenPropertyChanged.emit(token, Token_BorderWidth, TokenPropertyValue(borderWidth));

    glm::vec4 borderColor = token->GetBorderColor();
    if (ImGui::ColorEdit3("Border Colour", (float*)&borderColor))
        tokenPropertyChanged.emit(token, Token_BorderColor, TokenPropertyValue(borderColor));
    
    const std::shared_ptr<Matrix2D>& matrix2D = token->GetModel();

    glm::vec2 pos = matrix2D->GetPos();
    if (ImGui::DragFloat2("Position##Token", (float*)&pos))
        tokenPropertyChanged.emit(token, Token_Position, TokenPropertyValue(pos));

    float scale = matrix2D->GetScale().x;
    if (ImGui::SliderFloat("Size##Token", &scale, 0, 100, "%.3f", ImGuiSliderFlags_Logarithmic))
        tokenPropertyChanged.emit(token, Token_Scale, TokenPropertyValue(glm::vec2(scale, scale)));

    float rotation = matrix2D->GetRotation();
    if (ImGui::SliderFloat("Rotation##Token", &rotation, 0, 360, "%.2f"))
        tokenPropertyChanged.emit(token, Token_Rotation, TokenPropertyValue(rotation));

    // TODO: Mixed status for multiple tokens? Ideally only add/remove one if changed
    bool enabled = token->GetXStatus();
    if (ImGui::Checkbox("X", &enabled))
        tokenPropertyChanged.emit(token, Token_XStatus, TokenPropertyValue(enabled));

    TokenStatuses statuses = token->GetStatuses();
    for (size_t i = 0; i < statuses.size(); i++)
    {
        ImGui::SameLine();
        enabled = token->IsStatusEnabled(i);
        if (ImGui::Checkbox(tokenNames[i].c_str(), &enabled))
            statuses.set(i, enabled);
    }

    if (token->GetStatuses() != statuses)
        tokenPropertyChanged.emit(token, Token_Statuses, TokenPropertyValue(statuses));

    float opacity = token->GetOpacity();
    if (ImGui::SliderFloat("Opacity##Token", &opacity, 0.0f, 1.0f, "%.3f"))
        tokenPropertyChanged.emit(token, Token_Opacity, TokenPropertyValue(opacity));
}


void UIWindow::DrawCameraSection()
{
    if (ImGui::CollapsingHeader("Cameras"))
    {
        auto hostCamera = m_scene->GetViewCamera(PRIMARY);

        if (ImGui::BeginListBox("Cameras##List"))
        {
            for (const auto& camera: m_scene->cameras)
            {
                if (ImGui::Selectable(camera->GetName().c_str(), camera == hostCamera))
                    cameraSelectionChanged.emit(camera);
            }
            ImGui::EndListBox();
        }

        if (hostCamera)
        {
            std::string name = hostCamera->GetName();
            if (ImGui::InputText("Name", &name, ImGuiInputTextFlags_EnterReturnsTrue))
                cameraPropertyChanged.emit(hostCamera, Camera_Name, name);
        }

        if (ImGui::Button("Clone Camera"))
            cloneCameraClicked.emit();

        ImGui::SameLine();
        if (ImGui::Button("Delete Camera"))
            deleteCameraClicked.emit();
    }
}

void UIWindow::DrawGridSection()
{
    if (ImGui::CollapsingHeader("Grid"))
        DrawGridOptions(m_scene->grid);
}

void UIWindow::DrawImageSection()
{
    if (ImGui::CollapsingHeader("Images"))
    {
        bool lockImages = m_scene->GetImagesLocked();
        if (ImGui::Checkbox("Lock Images in Viewport", &lockImages))
            imageLockChanged.emit(lockImages);

        if (ImGui::BeginListBox("Images##List"))
        {
            int i = 0;
            for (const auto& image : m_scene->images)
            {
                bool isUISelected = image == m_displayPropertiesImage;
                if (isUISelected)
                {
                    ImGui::PushStyleColor(ImGuiCol_Header, SELECT_COLOR);
                    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, SELECT_COLOR);
                }
                if (ImGui::Selectable((image->GetImage()->Name() + "##Item" + std::to_string(i++)).c_str(), image->isSelected || isUISelected))
                    shapeSelectionChanged.emit(static_cast<std::shared_ptr<Shape2D>>(image), HasKeyPressed(GLFW_KEY_LEFT_CONTROL));

                if (isUISelected)
                    ImGui::PopStyleColor(2);
            }
            ImGui::EndListBox();
        }

        if (ImGui::Button("Add Image"))
            addImageClicked.emit();

        if (std::find(m_scene->images.begin(), m_scene->images.end(), m_displayPropertiesImage) != m_scene->images.end())
            DrawImageOptions(m_displayPropertiesImage);
        else
            m_displayPropertiesImage = nullptr;
    }

}

void UIWindow::DrawTokenSection()
{
    if (ImGui::CollapsingHeader("Token"))
    {
        bool lockTokens = m_scene->GetTokensLocked();
        if (ImGui::Checkbox("Lock Tokens in Viewport", &lockTokens))
            tokenLockChanged.emit(lockTokens);

        if (ImGui::BeginListBox("Tokens##List"))
        {
            int i = 0;
            for (std::shared_ptr<Token>& token : m_scene->tokens)
            {
                bool isUISelected = token == m_displayPropertiesToken;
                if (isUISelected)
                {
                    ImGui::PushStyleColor(ImGuiCol_Header, SELECT_COLOR);
                    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, SELECT_COLOR);
                }
                if (ImGui::Selectable((token->GetName() + "##Item" + std::to_string(i++)).c_str(), token->isSelected || isUISelected))
                    shapeSelectionChanged.emit(static_cast<std::shared_ptr<Shape2D>>(token), HasKeyPressed(GLFW_KEY_LEFT_CONTROL));

                if (isUISelected)
                    ImGui::PopStyleColor(2);
            }
            ImGui::EndListBox();
        }

        if (ImGui::Button("Add Token"))
            addTokenClicked.emit();

        if (std::find(m_scene->tokens.begin(), m_scene->tokens.end(), m_displayPropertiesToken) != m_scene->tokens.end())
            DrawTokenOptions(m_displayPropertiesToken);
        else
            m_displayPropertiesToken = nullptr;
    }
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

        DrawCameraSection();
        DrawGridSection();
        DrawImageSection();
        DrawTokenSection();

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

void UIWindow::OnKeyChanged(int key, int scancode, int action, int mods)
{
    ImGuiIO& io = ImGui::GetIO();
    if (!io.WantCaptureKeyboard)
        keyChanged.emit(key, scancode, action, mods);
}
