#ifndef APPLICATIONEVENT_H
#define APPLICATIONEVENT_H

#pragma once

#include "Events/Event.h"

namespace SGE {
    class WindowCloseEvent : public Event
    {
    public:
        WindowCloseEvent() = default;
        virtual std::string ToString() const { return "Window Close Event"; }
        
        EVENT_CLASS_CATEGORY(EventCategoryApplication);
        EVENT_CLASS_TYPE(WindowClose);
    };

    class WindowResizeEvent: public Event
    {
    public:
        WindowResizeEvent(int width, int height) 
            :m_Width(width), m_Height(height) {}

        virtual std::string ToString() const {
            std::stringstream ss;
            ss << "Width: " << m_Width << ", Height : " << m_Height;
            return ss.str();
         }
        
        EVENT_CLASS_CATEGORY(EventCategoryApplication);
        EVENT_CLASS_TYPE(WindowResize);

        int GetWidth() const {return m_Width;}
        int GetHeight() const {return m_Height;}
    private:
        int m_Width;
        int m_Height;
    };
}

#endif