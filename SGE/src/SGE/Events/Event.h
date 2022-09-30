#ifndef EVENT_H
#define EVENT_H

#pragma once
#include "Core/Core.h"

namespace SGE {
    enum class EventType
    {
        None = 0,
        WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMove,
        AppTick, AppUpdate, AppRender,
        KeyPress, KeyRelease, KeyType,
        MouseButtonPress, MouseButtonRelease,  MouseMove, MouseScroll
    };

    enum EventCategory
    {
        None = 0,
        EventCategoryApplication = BIT(0),
        EventCategoryInput       = BIT(1),
        EventCategoryKeyboard    = BIT(2),
        EventCategoryMouse       = BIT(3),
        EventCategoryMouseButton = BIT(4)
    };

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() {return EventType::type;}\
                               virtual EventType GetEventType() const override {return EventType::type;}\
                               virtual const char* GetName() const override {return #type;}

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlag() const override {return EventCategory::category;}
    class Event
    {
    public:
        virtual EventType GetEventType() const = 0;
        virtual int GetCategoryFlag() const = 0;
        virtual const char* GetName() const = 0;
        virtual std::string ToString() const = 0;

        inline bool IsInCategory(EventCategory category) { return category & GetCategoryFlag(); }
        inline void HandleEvent() {m_Handled = true;}

        bool m_Handled = false;
    };

    class EventDispatcher
    {
    public:
        template<typename T>
        using EventFn = std::function<bool(T&)>;

        // Create event dispatcher for certain event
        EventDispatcher(Event& event)
            :m_Event(event){}

        template<typename T>
        bool Dispatch(EventFn<T> func)
        {
            // Check if dispatchers' event matches the incoming event
            if(m_Event.GetEventType() == T::GetStaticType())
            {
                m_Event.m_Handled = func(*(T*)&m_Event);
            }
            return false;
        }
    private:
        Event& m_Event;
    };
}
#endif