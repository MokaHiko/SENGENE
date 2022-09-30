#ifndef MOUSEEVENT_H
#define MOUSEEVENT_H

#pragma once

#include "Events/Event.h"

namespace SGE {
    class MouseMoveEvent : public Event
    {
    public:
        MouseMoveEvent(float x, float y)
            :m_MouseX(x), m_MouseY(y) {}

        EVENT_CLASS_TYPE(MouseMove);
        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput);

        virtual std::string ToString() const
        {
            std::stringstream ss;
            ss << "MouseMoveEvent: " << m_MouseX << ", " << m_MouseY;
            return ss.str();
        }

        std::pair<float, float> GetMouseCoordinates() {return std::pair<float, float>(m_MouseX, m_MouseY);}
    private:
        float m_MouseX, m_MouseY;
    };


}


#endif