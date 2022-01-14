#pragma once
#include <functional>
#include <map>


template <typename... Args>
class Signal
{
public:
    int connect(std::function<void(Args...)> const& slot)
    {
        m_slots.emplace(++m_id, slot);
        return m_id;
    }

    template <typename T>
    int connect(T *inst, void (T::*func)(Args...))
    {
        return connect([=](Args... args) { 
            (inst->*func)(args...); 
        });
    }

    void disconnect()
    {
        m_slots.clear();
    }

    void disconnect(int id)
    {
        m_slots.erase(id);
    }

    void emit(Args... args)
    {
        for (const auto& it : m_slots)
            it.second(args...);
    }

private:
    int m_id = 0;
    std::map<int, std::function<void(Args...)>> m_slots;
};
