#ifndef FOOD_H
#define FOOD_H

#pragma once

#include "SGE/SGE.h"

struct FoodProperties
{
    int HealthRegen;
};

class Food : public SGE::ScriptableEntity
{
public:
    Food();
    ~Food();

    virtual void OnCreate()
    {
        m_Properties.HealthRegen = 10.0f;
    }

    virtual void OnUpdate(SGE::TimeStep timestep) override{

    };

    virtual void OnStart() override
    {
    }

private:
    FoodProperties m_Properties;
};

#endif