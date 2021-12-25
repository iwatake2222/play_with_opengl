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
#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "matrix.h"

namespace Transform
{
    Matrix Translate(float x, float y, float z);
    Matrix Scale(float x, float y, float z);
    Matrix RotateX(float rad);
    Matrix RotateY(float rad);
    Matrix RotateZ(float rad);
    Matrix Rotate(float rad, float x, float y, float z);
    Matrix LookAt(
        float eye_x, float eye_y, float eye_z,
        float gaze_x, float gaze_y, float gaze_z,
        float up_x, float up_y, float up_z);
    Matrix LookAt(Matrix eye, Matrix gaze, Matrix up);
}

namespace Projection {
    Matrix Orthogonal(float left, float right, float bottom, float top, float z_near, float z_far);
    Matrix Frustum(float left, float right, float bottom, float top, float z_near, float z_far);
    Matrix Perspective(float fovy, float aspect, float z_near, float z_far);
}

#endif
