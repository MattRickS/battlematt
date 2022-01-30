#pragma once
#include <chrono>
#include <memory>

#include <glm/glm.hpp>

#include <glutil/Matrix2D.h>
#include <glutil/Texture.h>
#include <model/BGImage.h>
#include <model/Grid.h>
#include <model/Scene.h>
#include <model/Token.h>


class Action
{
public:
    virtual void Undo() = 0;
    virtual void Redo() = 0;
    // Merge tries to combine Actions of the same type.
    virtual bool CanMerge(const std::shared_ptr<Action>& action) { return false; }
    virtual void Merge(const std::shared_ptr<Action>& action) {}
};


class ActionGroup: public Action
{
public:
    ActionGroup() {}
    ActionGroup(std::vector<std::shared_ptr<Action>> actions) : m_actions(actions) {}

    virtual void Undo()
    {
        for (auto& action: m_actions)
            action->Undo();
    }
    virtual void Redo()
    {
        for (auto& action: m_actions)
            action->Redo();
    }

    void Add(const std::shared_ptr<Action>& action)
    {
        m_actions.push_back(action);
    }
    bool IsEmpty() { return m_actions.empty(); }

    virtual bool CanMerge(const std::shared_ptr<Action>& action)
    {
        auto actionGroup = std::dynamic_pointer_cast<ActionGroup>(action);
        if (!actionGroup || actionGroup->m_actions.size() != m_actions.size())
            return false;

        for (unsigned int i = 0; i < m_actions.size(); i++)
        {
            if (!m_actions[i]->CanMerge(actionGroup->m_actions[i]))
                return false;
        }
        return true;
    }
    virtual void Merge(const std::shared_ptr<Action>& action)
    {
        auto actionGroup = std::dynamic_pointer_cast<ActionGroup>(action);
        for (unsigned int i = 0; i < m_actions.size(); i++)
            m_actions[i]->Merge(actionGroup->m_actions[i]);
    }

private:
    std::vector<std::shared_ptr<Action>> m_actions;
};


// Modify Properties
template <typename T, typename argT>
class ModifyMemberAction: public Action
{
public:
    ModifyMemberAction(const std::shared_ptr<T>& inst, void (T::*func)(argT), argT oldVal, argT newVal) :
        m_inst(inst), m_func(func), m_oldVal(oldVal), m_newVal(newVal) {}

    virtual void Undo()
    {
        (m_inst.get()->*m_func)(m_oldVal);
    }
    virtual void Redo()
    {
        (m_inst.get()->*m_func)(m_newVal);
    }
    virtual bool CanMerge(const std::shared_ptr<Action>& action)
    {
        auto modifyAction = std::dynamic_pointer_cast<ModifyMemberAction>(action);
        if (modifyAction && modifyAction->m_inst == m_inst && modifyAction->m_func == m_func && occurredAt - modifyAction->occurredAt < maxTimeBetweenUniqueActions)
            return true;

        return false;
    }
    virtual void Merge(const std::shared_ptr<Action>& action)
    {
        auto modifyAction = std::dynamic_pointer_cast<ModifyMemberAction>(action);
        m_newVal = modifyAction->m_newVal;
    }

private:
    std::shared_ptr<T> m_inst;
    void (T::*m_func)(argT);
    argT m_oldVal, m_newVal;
    const std::chrono::milliseconds maxTimeBetweenUniqueActions{500};
    std::chrono::steady_clock::time_point occurredAt = std::chrono::steady_clock::now();
};

typedef ModifyMemberAction<Grid, bool> ModifyGridBool;  // snap
typedef ModifyMemberAction<Grid, float> ModifyGridFloat;  // size
typedef ModifyMemberAction<Grid, glm::vec3> ModifyGridVec3;  // colour

typedef ModifyMemberAction<Matrix2D, glm::vec2> ModifyMatrix2DVec2;  // pos, scale
typedef ModifyMemberAction<Matrix2D, float> ModifyMatrix2DFloat;  // rotation

typedef ModifyMemberAction<BGImage, bool> ModifyImageBool;  // lockRatio
typedef ModifyMemberAction<BGImage, std::shared_ptr<Texture>> ModifyImageTexture;  // image

typedef ModifyMemberAction<Token, bool> ModifyTokenBool;  // xStatus
typedef ModifyMemberAction<Token, float> ModifyTokenFloat;  // borderWidth
typedef ModifyMemberAction<Token, glm::vec4> ModifyTokenVec4;  // borderColour
typedef ModifyMemberAction<Token, std::shared_ptr<Texture>> ModifyTokenTexture;  // icon
typedef ModifyMemberAction<Token, std::string> ModifyTokenString;  // name
typedef ModifyMemberAction<Token, TokenStatuses> ModifyTokenStatuses;

typedef ModifyMemberAction<Scene, bool> ModifySceneLocks;

// Selection
class SelectShapesAction : public Action
{
public:
    SelectShapesAction(std::vector<std::shared_ptr<Shape2D>> selected) : selectedShapes(selected) {}
    SelectShapesAction(std::vector<std::shared_ptr<Shape2D>> selected, const std::shared_ptr<Shape2D>& token, bool add=false) : selectedShapes(selected), m_additive(add)
    {
        shapesToSelect.push_back(token);
    }
    SelectShapesAction(std::vector<std::shared_ptr<Shape2D>> selected, std::vector<std::shared_ptr<Shape2D>> toSelect, bool add=false) : selectedShapes(selected), shapesToSelect(toSelect), m_additive(add) {}

    virtual void Undo()
    {
        if (m_additive)
            Select(shapesToSelect, false);
        else
        {
            Select(shapesToSelect, false);
            Select(selectedShapes, true);
        }
    }
    virtual void Redo()
    {
        if (!m_additive)
            Select(selectedShapes, false);
        Select(shapesToSelect, true);
    }

    // If selecting the same thing / selecting nothing, then the actions can be combined
    virtual bool CanMerge(const std::shared_ptr<Action>& action)
    {
        auto selectionAction = std::dynamic_pointer_cast<SelectShapesAction>(action);
        if (selectionAction && selectionAction->shapesToSelect == shapesToSelect)
            return true;

        return false;
    }
    virtual void Merge(const std::shared_ptr<Action>& action) {}

private:
    std::vector<std::shared_ptr<Shape2D>> selectedShapes;
    std::vector<std::shared_ptr<Shape2D>> shapesToSelect;
    bool m_additive = false;

    void Select(const std::vector<std::shared_ptr<Shape2D>>& shapes, bool select)
    {
        for (const auto& shape: shapes)
            shape->isSelected = select;
    }
};

// Add/Remove
class AddTokensAction : public Action
{
public:
    AddTokensAction(const std::shared_ptr<Scene>& scene) : m_scene(scene) {}
    AddTokensAction(const std::shared_ptr<Scene>& scene, const std::shared_ptr<Token>& token) : m_scene(scene)
    {
        m_tokens.push_back(token);
    }
    AddTokensAction(const std::shared_ptr<Scene>& scene, std::vector<std::shared_ptr<Token>> tokens) : m_scene(scene), m_tokens(tokens) {}

    void Add(const std::shared_ptr<Token>& token)
    {
        m_tokens.push_back(token);
    }

    virtual void Undo()
    {
        m_scene->RemoveTokens(m_tokens);
    }
    virtual void Redo()
    {
        for (const auto& token: m_tokens)
            m_scene->AddToken(token);
    }

private:
    std::shared_ptr<Scene> m_scene;
    std::vector<std::shared_ptr<Token>> m_tokens;
};

class RemoveTokensAction : public Action
{
public:
    RemoveTokensAction(const std::shared_ptr<Scene>& scene) : m_scene(scene) {}
    RemoveTokensAction(const std::shared_ptr<Scene>& scene, const std::shared_ptr<Token>& token) : m_scene(scene)
    {
        m_tokens.push_back(token);
    }
    RemoveTokensAction(const std::shared_ptr<Scene>& scene, std::vector<std::shared_ptr<Token>> tokens) : m_scene(scene), m_tokens(tokens) {}

    void Add(const std::shared_ptr<Token>& token)
    {
        m_tokens.push_back(token);
    }

    virtual void Undo()
    {
        for (const auto& token: m_tokens)
            m_scene->AddToken(token);
    }
    virtual void Redo()
    {
        m_scene->RemoveTokens(m_tokens);
    }

private:
    std::shared_ptr<Scene> m_scene;
    std::vector<std::shared_ptr<Token>> m_tokens;
};

class AddImagesAction : public Action
{
public:
    AddImagesAction(const std::shared_ptr<Scene>& scene) : m_scene(scene) {}
    AddImagesAction(const std::shared_ptr<Scene>& scene, const std::shared_ptr<BGImage>& image) : m_scene(scene)
    {
        m_images.push_back(image);
    }
    AddImagesAction(const std::shared_ptr<Scene>& scene, std::vector<std::shared_ptr<BGImage>> images) : m_scene(scene), m_images(images) {}

    void Add(const std::shared_ptr<BGImage>& image)
    {
        m_images.push_back(image);
    }

    virtual void Undo()
    {
        m_scene->RemoveImages(m_images);
    }
    virtual void Redo()
    {
        for (const auto& image: m_images)
            m_scene->AddImage(image);
    }

private:
    std::shared_ptr<Scene> m_scene;
    std::vector<std::shared_ptr<BGImage>> m_images;
};

class RemoveImagesAction : public Action
{
public:
    RemoveImagesAction(const std::shared_ptr<Scene>& scene) : m_scene(scene) {}
    RemoveImagesAction(const std::shared_ptr<Scene>& scene, const std::shared_ptr<BGImage>& image) : m_scene(scene)
    {
        m_images.push_back(image);
    }
    RemoveImagesAction(const std::shared_ptr<Scene>& scene, std::vector<std::shared_ptr<BGImage>> images) : m_scene(scene), m_images(images) {}

    void Add(const std::shared_ptr<BGImage>& image)
    {
        m_images.push_back(image);
    }

    virtual void Undo()
    {
        for (const auto& image: m_images)
            m_scene->AddImage(image);
    }
    virtual void Redo()
    {
        m_scene->RemoveImages(m_images);
    }

private:
    std::shared_ptr<Scene> m_scene;
    std::vector<std::shared_ptr<BGImage>> m_images;
};
