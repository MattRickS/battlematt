#pragma once
#include <memory>
#include <string>

#include <Actions.hpp>
#include <Resources.h>
#include <Signal.hpp>
#include <glutil/Matrix2D.h>
#include <model/BGImage.h>
#include <model/Grid.h>
#include <model/Scene.h>
#include <model/Shape2D.h>
#include <model/Token.h>
#include <view/Properties.h>
#include <view/Window.h>


class UIControls
{
public:
    Signal<> addImageClicked;
    Signal<const std::shared_ptr<BGImage>&> removeImageClicked;
    Signal<> addTokenClicked;
    Signal<std::string> saveClicked;
    Signal<std::string, bool> loadClicked;
    Signal<int, bool> promptResponse;
    Signal<std::shared_ptr<Shape2D>, bool> shapeSelectionChanged;
    Signal<const std::shared_ptr<Token>&, TokenProperty, TokenPropertyValue> tokenPropertyChanged;
    Signal<const std::shared_ptr<BGImage>&, ImageProperty, ImagePropertyValue> imagePropertyChanged;
    Signal<const std::shared_ptr<Grid>&, GridProperty, GridPropertyValue> gridPropertyChanged;
    Signal<const std::shared_ptr<Camera>&, CameraProperty, CameraPropertyValue> cameraPropertyChanged;
    Signal<bool> imageLockChanged;
    Signal<bool> tokenLockChanged;
    Signal<const std::shared_ptr<Camera>&> cameraSelectionChanged;
    Signal<> cloneCameraClicked;
    Signal<> deleteCameraClicked;

    UIControls(const std::shared_ptr<Window>& window);
    ~UIControls();

    void Draw();
    void SetScene(std::shared_ptr<Scene> scene);
    void Prompt(int promptType, std::string msg);
    bool HasPrompt();

    void SetDisplayPropertiesToken(const std::shared_ptr<Token>& token);
    void SetDisplayPropertiesImage(const std::shared_ptr<BGImage>& image);

private:
    const std::shared_ptr<Window>& m_window = nullptr;
    std::shared_ptr<Scene> m_scene = nullptr;
    std::string m_promptMsg = "";
    int m_promptType = 0;
    bool mergeLoad = false;

    std::string tokenNames[NUM_TOKEN_STATUSES] {"Red", "Green", "Blue", "Yellow", "Cyan", "Pink"};

    std::shared_ptr<Token> m_displayPropertiesToken = nullptr;
    std::shared_ptr<BGImage> m_displayPropertiesImage = nullptr;

    void DrawImageOptions(const std::shared_ptr<BGImage>& image);
    void DrawGridOptions(const std::shared_ptr<Grid>&);
    void DrawTokenOptions(const std::shared_ptr<Token>& tokens);

    void DrawCameraSection();
    void DrawGridSection();
    void DrawImageSection();
    void DrawTokenSection();

    void RespondToPrompt(bool response);

};
