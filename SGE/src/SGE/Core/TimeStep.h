#ifndef TIMESTEP_H
#define TIMESTEP_H

#pragma once

namespace SGE {
    /*
        Time is always in seconds unless specified
    */

    class TimeStep
    {
    public:
        TimeStep(float time = 0.0f)
            :m_Time(time)
        {
        }
        ~TimeStep() = default;

        operator float() const {return m_Time;}
        
        float GetSeconds() const {return m_Time;}
        float GetMilliSeconds() const {return m_Time * 1000.0f;}
    private:
        float m_Time;
    };
}

#endif