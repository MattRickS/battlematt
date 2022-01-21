#pragma once
#include <memory>
#include <vector>

#include <model/Overlays.h>
#include <model/Token.h>


struct UIState
{
    std::shared_ptr<RectOverlay> dragSelectRect = nullptr;
    std::shared_ptr<Token> tokenUnderCursor = nullptr;
};