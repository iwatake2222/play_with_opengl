/* Copyright 2021 iwatake2222

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#ifndef OBJECT_DATA_H
#define OBJECT_DATA_H

/*** Include ***/
/* for general */
#include <cstdint>
#include <cstdio>
#include <vector>
#include <array>

#include "Shape.h"

Shape* CreateGround(float size, float interval, std::array<float, 3> color_vec3 = { 0.0f, 0.5f, 0.5f });
Shape* CreateArrowZ(float size, float arrow_size, std::array<float, 3> color_vec3 = { 0.5f, 0.0f, 0.0f });
Shape* CreateAxes(float size, float arrow_size, std::array<float, 3> color_x, std::array<float, 3> color_y, std::array<float, 3> color_z);
Shape* CreateFlatObject(float width, float height, float thickness, std::array<float, 3> color_front, std::array<float, 3> color_back);

const std::vector<Object::Vertex> CubeWireVertex
{
    { -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f },
    { -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.8f },
    { -1.0f, 1.0f, 1.0f, 0.0f, 0.8f, 0.0f },
    { -1.0f, 1.0f, -1.0f, 0.0f, 0.8f, 0.8f },
    { 1.0f, 1.0f, -1.0f, 0.8f, 0.0f, 0.0f },
    { 1.0f, -1.0f, -1.0f, 0.8f, 0.0f, 0.8f },
    { 1.0f, -1.0f, 1.0f, 0.8f, 0.8f, 0.0f },
    { 1.0f, 1.0f, 1.0f, 0.8f, 0.8f, 0.8f }
};

const std::vector<GLuint> CubeWireIndex
{
    1, 0,
    2, 7,
    3, 0,
    4, 7,
    5, 0,
    6, 7,
    1, 2,
    2, 3,
    3, 4,
    4, 5,
    5, 6,
    6, 1
};

const std::vector<Object::Vertex> CubeTriangleVertex
{
    { -1.0f, -1.0f, -1.0f, 0.1f, 0.8f, 0.1f },
    { -1.0f, -1.0f, 1.0f, 0.1f, 0.8f, 0.1f },
    { -1.0f, 1.0f, 1.0f, 0.1f, 0.8f, 0.1f },
    { -1.0f, -1.0f, -1.0f, 0.1f, 0.8f, 0.1f },
    { -1.0f, 1.0f, 1.0f, 0.1f, 0.8f, 0.1f },
    { -1.0f, 1.0f, -1.0f, 0.1f, 0.8f, 0.1f },

    { 1.0f, -1.0f, -1.0f, 0.8f, 0.1f, 0.8f },
    { -1.0f, -1.0f, -1.0f, 0.8f, 0.1f, 0.8f },
    { -1.0f, 1.0f, -1.0f, 0.8f, 0.1f, 0.8f },
    { 1.0f, -1.0f, -1.0f, 0.8f, 0.1f, 0.8f },
    { -1.0f, 1.0f, -1.0f, 0.8f, 0.1f, 0.8f },
    { 1.0f, 1.0f, -1.0f, 0.8f, 0.1f, 0.8f },

    { -1.0f, -1.0f, -1.0f, 0.1f, 0.8f, 0.8f },
    { 1.0f, -1.0f, -1.0f, 0.1f, 0.8f, 0.8f },
    { 1.0f, -1.0f, 1.0f, 0.1f, 0.8f, 0.8f },
    { -1.0f, -1.0f, -1.0f, 0.1f, 0.8f, 0.8f },
    { 1.0f, -1.0f, 1.0f, 0.1f, 0.8f, 0.8f },
    { -1.0f, -1.0f, 1.0f, 0.1f, 0.8f, 0.8f },

    { 1.0f, -1.0f, 1.0f, 0.1f, 0.1f, 0.8f },
    { 1.0f, -1.0f, -1.0f, 0.1f, 0.1f, 0.8f },
    { 1.0f, 1.0f, -1.0f, 0.1f, 0.1f, 0.8f },
    { 1.0f, -1.0f, 1.0f, 0.1f, 0.1f, 0.8f },
    { 1.0f, 1.0f, -1.0f, 0.1f, 0.1f, 0.8f },
    { 1.0f, 1.0f, 1.0f, 0.1f, 0.1f, 0.8f },

    { -1.0f, 1.0f, -1.0f, 0.8f, 0.1f, 0.1f },
    { -1.0f, 1.0f, 1.0f, 0.8f, 0.1f, 0.1f },
    { 1.0f, 1.0f, 1.0f, 0.8f, 0.1f, 0.1f },
    { -1.0f, 1.0f, -1.0f, 0.8f, 0.1f, 0.1f },
    { 1.0f, 1.0f, 1.0f, 0.8f, 0.1f, 0.1f },
    { 1.0f, 1.0f, -1.0f, 0.8f, 0.1f, 0.1f },

    { -1.0f, -1.0f, 1.0f, 0.8f, 0.8f, 0.1f },
    { 1.0f, -1.0f, 1.0f, 0.8f, 0.8f, 0.1f },
    { 1.0f, 1.0f, 1.0f, 0.8f, 0.8f, 0.1f },
    { -1.0f, -1.0f, 1.0f, 0.8f, 0.8f, 0.1f },
    { 1.0f, 1.0f, 1.0f, 0.8f, 0.8f, 0.1f },
    { -1.0f, 1.0f, 1.0f, 0.8f, 0.8f, 0.1f }
};

const std::vector<GLuint> CubeTriangleIndex
{
    0, 1, 2, 3, 4, 5,
    6, 7, 8, 9, 10, 11,
    12, 13, 14, 15, 16, 17,
    18, 19, 20, 21, 22, 23,
    24, 25, 26, 27, 28, 29,
    30, 31, 32, 33, 34, 35
};

#endif
