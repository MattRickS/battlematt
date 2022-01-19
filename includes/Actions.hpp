#pragma once
#include <memory>

#include <glm/glm.hpp>

#include <glutil/Texture.h>
#include <model/BGImage.h>
#include <model/Grid.h>
#include <model/Token.h>


class Action
{
public:
    virtual void Undo() {}
    virtual void Redo() {}
};


class ActionGroup: public Action
{
public:
    ActionGroup(std::vector<Action> actions) : m_actions(actions) {}

    virtual void Undo()
    {
        for (auto& action: m_actions)
            action.Undo();
    }
    virtual void Redo()
    {
        for (auto& action: m_actions)
            action.Redo();
    }

private:
    std::vector<Action> m_actions;
};


template <typename T, typename argT>
class ModifyMemberAction: public Action
{
public:
    ModifyMemberAction(std::shared_ptr<T> inst, void (T::*func)(argT), argT oldVal, argT newVal) :
        m_inst(inst), m_func(func), m_oldVal(oldVal), m_newVal(newVal) {}

    virtual void Undo()
    {
        (m_inst.get()->*m_func)(m_oldVal);
    }
    virtual void Redo()
    {
        (m_inst.get()->*m_func)(m_newVal);
    }

private:
    std::shared_ptr<T> m_inst;
    void (T::*m_func)(argT);
    argT m_oldVal, m_newVal;
};

template class ModifyMemberAction<Token, float>;  // rotation, borderWidth
template class ModifyMemberAction<Token, glm::vec2>;  // pos, size
template class ModifyMemberAction<Token, glm::vec4>;  // borderColour
template class ModifyMemberAction<Token, std::shared_ptr<Texture>>;  // icon
template class ModifyMemberAction<Token, std::string>;  // name

template class ModifyMemberAction<BGImage, bool>;  // lockRatio
template class ModifyMemberAction<BGImage, float>;  // rotation
template class ModifyMemberAction<BGImage, glm::vec2>;  // pos, size
template class ModifyMemberAction<BGImage, std::shared_ptr<Texture>>;  // image

template class ModifyMemberAction<Grid, float>;  // size
template class ModifyMemberAction<Grid, glm::vec4>;  // colour
