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
#include <view/Viewport.h>
#include <view/Window.h>


class UIWindow : public Viewport
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
    Signal<bool> imageLockChanged;
    Signal<bool> tokenLockChanged;
    Signal<int> cameraIndexChanged;
    Signal<> cloneCameraClicked;
    Signal<> deleteCameraClicked;

    UIWindow(unsigned int width, unsigned int height, std::shared_ptr<Resources> resources, std::shared_ptr<Window> share = NULL);
    ~UIWindow();

    virtual void Draw();
    void Prompt(int promptType, std::string msg);
    bool HasPrompt();

    void SetDisplayPropertiesToken(const std::shared_ptr<Token>& token);
    void SetDisplayPropertiesImage(const std::shared_ptr<BGImage>& image);

    virtual void OnKeyChanged(int key, int scancode, int action, int mods);

private:
    std::shared_ptr<Resources> m_resources;
    std::string m_promptMsg = "";
    int m_promptType = 0;
    bool mergeLoad = false;

    std::string tokenNames[NUM_TOKEN_STATUSES] {"Red", "Green", "Blue", "Yellow", "Cyan", "Pink"};

    std::shared_ptr<Token> m_displayPropertiesToken = nullptr;
    std::shared_ptr<BGImage> m_displayPropertiesImage = nullptr;

    void DrawImageOptions(const std::shared_ptr<BGImage>& image);
    void DrawGridOptions(const std::shared_ptr<Grid>&);
    void DrawTokenOptions(const std::shared_ptr<Token>& tokens);

    void RespondToPrompt(bool response);

};
