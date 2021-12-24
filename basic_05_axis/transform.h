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
}

namespace Projection {
    Matrix Orthogonal(float left, float right, float bottom, float top, float z_near, float z_far);
    Matrix Frustum(float left, float right, float bottom, float top, float z_near, float z_far);
    Matrix Perspective(float fovy, float aspect, float z_near, float z_far);
}

#endif
