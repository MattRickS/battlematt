#pragma once
#include <memory>

#include <glm/glm.hpp>

#include <glutil/Matrix2D.h>
#include <glutil/Texture.h>
#include <model/BGImage.h>
#include <model/Grid.h>
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
        if (modifyAction && modifyAction->m_inst == m_inst && modifyAction->m_func == m_func)
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
};


typedef ModifyMemberAction<Grid, bool> ModifyGridBool;  // snap
typedef ModifyMemberAction<Grid, float> ModifyGridFloat;  // size
typedef ModifyMemberAction<Grid, glm::vec3> ModifyGridVec3;  // colour

typedef ModifyMemberAction<Matrix2D, glm::vec2> ModifyMatrix2DVec2;  // pos, scale
typedef ModifyMemberAction<Matrix2D, float> ModifyMatrix2DFloat;  // rotation

typedef ModifyMemberAction<BGImage, bool> ModifyImageBool;  // lockRatio
typedef ModifyMemberAction<BGImage, std::shared_ptr<Texture>> ModifyImageTexture;  // image

typedef ModifyMemberAction<Token, float> ModifyTokenFloat;  // borderWidth
typedef ModifyMemberAction<Token, glm::vec4> ModifyTokenVec4;  // borderColour
typedef ModifyMemberAction<Token, std::shared_ptr<Texture>> ModifyTokenTexture;  // icon
typedef ModifyMemberAction<Token, std::string> ModifyTokenString;  // name
