#ifndef SCRIPTABLEENTITY_H
#define SCRIPTABLEENTITY_H

#pragma once
#include "Entity.h"
#include "Core/TimeStep.h"

namespace SGE {
    class ScriptableEntity
    {
    public:
        virtual ~ScriptableEntity() {};

        template<typename T>
        T& GetComponent()
        {
            return m_Entity.GetComponent<T>();
        }

        template<typename T, typename ... Args>
        T& AddComponent(Args&& ... args)
        {
            return m_Entity.AddComponent<T>(std::foward<Args>(args)...);
        }

        virtual void OnCreate(){};
        virtual void OnStart(){};
        virtual void OnDestroy(){};
        virtual void OnUpdate(TimeStep timestep){};
    private:
        Entity m_Entity{};
        friend class Scene;
    };
}

#endif