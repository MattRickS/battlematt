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
    Signal<> addTokenClicked;
    Signal<std::string> saveClicked;
    Signal<std::string> loadClicked;

    std::shared_ptr<UIState> uiState = nullptr;

    UIWindow(unsigned int width, unsigned int height, std::shared_ptr<Resources> resources, std::shared_ptr<Window> share = NULL);
    ~UIWindow();

    virtual void Draw();
    void SetScene(std::shared_ptr<Scene> scene);

private:
    std::shared_ptr<Resources> m_resources;
    std::shared_ptr<Scene> m_scene = nullptr;

    void DrawMatrix2DOptions(std::string suffixID, Matrix2D* matrix2D);
    void DrawShape2DOptions(std::string suffixID, std::vector<std::shared_ptr<Shape2D>>& shapes, std::shared_ptr<Grid> grid, bool snapToGrid = false, bool singleScale = false);
    void DrawBackgroundOptions(std::shared_ptr<BGImage> background, glm::vec4* bgColor);
    void DrawGridOptions(std::shared_ptr<Grid>);
    void DrawTokenOptions(std::vector<std::shared_ptr<Token>> tokens, std::shared_ptr<Grid> grid, bool snapToGrid = false);
};
