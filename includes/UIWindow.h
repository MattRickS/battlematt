#pragma once
#include <memory>
#include <string>

#include <BGImage.h>
#include <Grid.h>
#include <Matrix2D.h>
#include <Resources.h>
#include <Scene.h>
#include <Shape2D.h>
#include <Signal.hpp>
#include <Token.h>
#include <UIState.h>
#include <Window.h>


class UIWindow : public Window
{
public:
    Signal<> addImageClicked;
    Signal<> addTokenClicked;
    Signal<std::string> saveClicked;
    Signal<std::string, bool> loadClicked;
    Signal<int, bool> promptResponse;

    std::shared_ptr<UIState> uiState = nullptr;

    UIWindow(unsigned int width, unsigned int height, std::shared_ptr<Resources> resources, std::shared_ptr<Window> share = NULL);
    ~UIWindow();

    virtual void Draw();
    void SetScene(std::shared_ptr<Scene> scene);
    void Prompt(int promptType, std::string msg);
    bool HasPrompt();

private:
    std::shared_ptr<Resources> m_resources;
    std::shared_ptr<Scene> m_scene = nullptr;
    std::string m_promptMsg = "";
    int m_promptType = 0;
    bool mergeLoad = false;

    void DrawMatrix2DOptions(std::string suffixID, Matrix2D* matrix2D, bool lockScaleRatio=false);
    void DrawShape2DOptions(std::string suffixID, std::vector<std::shared_ptr<Shape2D>>& shapes, std::shared_ptr<Grid> grid, bool snapToGrid = false, bool singleScale = false);
    void DrawBackgroundOptions(std::shared_ptr<BGImage> background);
    void DrawGridOptions(std::shared_ptr<Grid>);
    void DrawTokenOptions(std::vector<std::shared_ptr<Token>> tokens, std::shared_ptr<Grid> grid, bool snapToGrid = false);

    void RespondToPrompt(bool response);
};
