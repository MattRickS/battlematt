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
#include <view/UIState.h>
#include <view/Window.h>


class UIWindow : public Window
{
public:
    Signal<> addImageClicked;
    Signal<> addTokenClicked;
    Signal<std::string> saveClicked;
    Signal<std::string, bool> loadClicked;
    Signal<int, bool> promptResponse;
    Signal<std::shared_ptr<Token>, bool> tokenSelectionChanged;
    Signal<const std::shared_ptr<Token>&, TokenProperty, TokenPropertyValue> tokenPropertyChanged;

    std::shared_ptr<UIState> uiState = nullptr;

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

    void DrawMatrix2DOptions(std::string suffixID, const std::shared_ptr<Matrix2D>& matrix2D, bool lockScaleRatio=false);
    void DrawShape2DOptions(std::string suffixID, std::shared_ptr<Shape2D> shapes, std::shared_ptr<Grid> grid, bool snapToGrid = false, bool singleScale = false);
    void DrawImageOptions(std::shared_ptr<BGImage> image);
    void DrawGridOptions(std::shared_ptr<Grid>);
    void DrawTokenOptions(std::shared_ptr<Token> tokens, std::shared_ptr<Grid> grid, bool snapToGrid = false);

    void RespondToPrompt(bool response);

    template <typename argT>
    bool IsStillEditing(argT& oldVal, const argT& newVal);
};
