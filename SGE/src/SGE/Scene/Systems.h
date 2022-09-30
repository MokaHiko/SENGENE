#ifndef SYSTEMS_H
#define SYSTEMS_H

#pragma once

#include "Scene/Components.h"

namespace SGE {
    void Camera3DSystem(Camera3DComponent& camera, TransformComponent& transform)
    {
        auto& cam3D = camera.camera;
        cam3D.UpdateCameraVectors();
        cam3D.CalculateViewMatrix(transform.Position);
    }
}

#endif