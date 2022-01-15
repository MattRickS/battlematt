#pragma once
#include <memory>
#include <vector>

#include <Overlays.h>
#include <Token.h>


struct UIState
{
    bool snapToGrid = false;
    std::vector<std::shared_ptr<Token>> selectedTokens = std::vector<std::shared_ptr<Token>>();
    std::unique_ptr<RectOverlay> dragSelectRect = nullptr;
    std::shared_ptr<Token> tokenUnderCursor = nullptr;
};