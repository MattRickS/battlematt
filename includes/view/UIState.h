#pragma once
#include <memory>
#include <vector>

#include <Overlays.h>
#include <Token.h>


struct UIState
{
    bool snapToGrid = false;
    std::shared_ptr<RectOverlay> dragSelectRect = nullptr;
    std::shared_ptr<Token> tokenUnderCursor = nullptr;
};