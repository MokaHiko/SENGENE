#ifndef FOOD_H
#define FOOD_H

#pragma once

#include "SGE/SGE.h"

struct FoodProperties
{
    float HealthRegen;
};

class Food : public SGE::ScriptableEntity
{
public:
    Food();
    ~Food();

    virtual void OnCreate()
    {
        Reset();
        m_Properties.HealthRegen = 10.0f;
    }

    virtual void OnUpdate(SGE::TimeStep timestep) override{

    };

    virtual void OnStart() override
    {
    }

public:
    FoodProperties Eat()
    {
        Reset();
        return m_Properties;
    }

    void Reset()
    {
        GameObject().GetComponent<SGE::RigidBodyComponent>().Body.SetPosition(glm::vec3{(rand() - RAND_MAX / 2) % m_SpawnRange, 0.0f, (rand() - RAND_MAX / 2) % m_SpawnRange});
    }

private:
    FoodProperties m_Properties;
    int m_SpawnRange = 20;
};

#endif