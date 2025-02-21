#pragma once

#include "glm/ext/matrix_clip_space.hpp"


enum class ProjectionType : uint8_t {
    Perspective,
    Orthographic
};


struct Settings {
    float fieldOfView = glm::radians(70.f);

    ProjectionType projectionType = ProjectionType::Perspective;

};


