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


class UIWindow : public Window
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

    UIWindow(unsigned int width, unsigned int height, std::shared_ptr<Resources> resources, std::shared_ptr<Window> share = NULL);
    ~UIWindow();

    virtual void Draw();
    void SetScene(std::shared_ptr<Scene> scene);
    void Prompt(int promptType, std::string msg);
    bool HasPrompt();

    virtual void OnKeyChanged(int key, int scancode, int action, int mods);

private:
    std::shared_ptr<Resources> m_resources;
    std::shared_ptr<Scene> m_scene = nullptr;
    std::string m_promptMsg = "";
    int m_promptType = 0;
    bool mergeLoad = false;

    std::string tokenNames[NUM_TOKEN_STATUSES] {"Red", "Green", "Blue", "Yellow", "Cyan", "Pink"};

    void DrawImageOptions(const std::shared_ptr<BGImage>& image);
    void DrawGridOptions(const std::shared_ptr<Grid>&);
    void DrawTokenOptions(const std::shared_ptr<Token>& tokens);

    void RespondToPrompt(bool response);

};
