/*** Include ***/
/* for general */
#include <cstdint>
#include <cstdio>
#include <fstream> 
#include <vector>
#include <string>
#include <memory>
#include <algorithm>

#include "transform.h"


/*** Function ***/
Matrix Transform::Translate(float x, float y, float z)
{
    Matrix mat = Matrix::Identity(4);
    mat[3] = x;
    mat[7] = y;
    mat[11] = z;
    return mat;
}

Matrix Transform::Scale(float x, float y, float z)
{
    Matrix mat = Matrix::Identity(4);
    mat[0] = x;
    mat[5] = y;
    mat[10] = z;
    return mat;
}

Matrix Transform::RotateX(float rad)
{
    Matrix mat = Matrix::Identity(4);
    mat[5] = std::cos(rad);
    mat[6] = -std::sin(rad);
    mat[9] = std::sin(rad);
    mat[10] = std::cos(rad);
    return mat;
}

Matrix Transform::RotateY(float rad)
{
    Matrix mat = Matrix::Identity(4);
    mat[0] = std::cos(rad);
    mat[2] = std::sin(rad);
    mat[8] = -std::sin(rad);
    mat[10] = std::cos(rad);
    return mat;
}

Matrix Transform::RotateZ(float rad)
{
    Matrix mat = Matrix::Identity(4);
    mat[0] = std::cos(rad);
    mat[1] = -std::sin(rad);
    mat[4] = std::sin(rad);
    mat[5] = std::cos(rad);
    return mat;
}

Matrix Transform::Rotate(float rad, float x, float y, float z)
{
    Matrix mat = Matrix::Identity(4);
    const float d = std::sqrt(x * x + y * y + z * z);
    if (d > 0.0f) {
        const float l = x / d;
        const float m = y / d;
        const float n = z / d;
        const float l2(l * l);
        const float m2 = m * m;
        const float n2 = n * n;
        const float lm = l * m;
        const float mn = m * n;
        const float nl = n * l;
        const float c = std::cos(rad);
        const float s = std::sin(rad);
        const float c1 = 1.0f - c;
        mat[0] = (1.0f - l2) * c + l2;
        mat[1] = lm * c1 - n * s;
        mat[2] = nl * c1 + m * s;
        mat[4] = lm * c1 + n * s;
        mat[5] = (1.0f - m2) * c + m2;
        mat[6] = mn * c1 - l * s;
        mat[8] = nl * c1 - m * s;
        mat[9] = mn * c1 + l * s;
        mat[10] = (1.0f - n2) * c + n2;
    }
    return mat;
}

Matrix Transform::LookAt(
    float eye_x, float eye_y, float eye_z,
    float gaze_x, float gaze_y, float gaze_z,
    float up_x, float up_y, float up_z)
{
    const Matrix tv(Translate(-eye_x, -eye_y, -eye_z));

    const float tx = eye_x - gaze_x;
    const float ty = eye_y - gaze_y;
    const float tz = eye_z - gaze_z;
    const float rx = up_y * tz - up_z * ty;
    const float ry = up_z * tx - up_x * tz;
    const float rz = up_x * ty - up_y * tx;
    const float sx = ty * rz - tz * ry;
    const float sy = tz * rx - tx * rz;
    const float sz = tx * ry - ty * rx;

    const float s = std::sqrt(sx * sx + sy * sy + sz * sz);
    if (s == 0.0f) return tv;
    Matrix rv = Matrix::Identity(4);
    const float r = std::sqrt(rx * rx + ry * ry + rz * rz);
    const float t = std::sqrt(tx * tx + ty * ty + tz * tz);
    rv[0] = rx / r;
    rv[1] = ry / r;
    rv[2] = rz / r;
    rv[4] = sx / s;
    rv[5] = sy / s;
    rv[6] = sz / s;
    rv[8] = tx / t;
    rv[9] = ty / t;
    rv[10] = tz / t;


    return rv * tv;
}


Matrix Projection::Orthogonal(float left, float right, float bottom, float top, float z_near, float z_far)
{
    Matrix mat = Matrix::Identity(4);
    const float dx = right - left;
    const float dy = top - bottom;
    const float dz = z_far - z_near;
    if (dx != 0.0f && dy != 0.0f && dz != 0.0f) {
        mat[0] = 2.0f / dx;
        mat[5] = 2.0f / dy;
        mat[10] = -2.0f / dz;
        mat[3] = -(right + left) / dx;
        mat[7] = -(top + bottom) / dy;
        mat[11] = -(z_far + z_near) / dz;
    }
    return mat;
}

Matrix Projection::Frustum(float left, float right, float bottom, float top, float z_near, float z_far)
{
    Matrix mat = Matrix::Identity(4);
    const float dx = right - left;
    const float dy = top - bottom;
    const float dz = z_far - z_near;
    if (dx != 0.0f && dy != 0.0f && dz != 0.0f)
    {
        mat[0] = 2.0f * z_near / dx;
        mat[5] = 2.0f * z_near / dy;
        mat[2] = (right + left) / dx;
        mat[6] = (top + bottom) / dy;
        mat[10] = -(z_far + z_near) / dz;
        mat[11] = -2.0f * z_far * z_near / dz;
        mat[14] = -1.0f;
        mat[15] = 0.0f;
    }
    return mat;
}
Matrix Projection::Perspective(float fovy, float aspect, float z_near, float z_far)
{
    Matrix mat = Matrix::Identity(4);
    const float dz = z_far - z_near;
    if (dz != 0.0f) {
        mat[5] = 1.0f / std::tan(fovy * 0.5f);
        mat[0] = mat[5] / aspect;
        mat[10] = -(z_far + z_near) / dz;
        mat[11] = -2.0f * z_far * z_near / dz;
        mat[14] = -1.0f;
        mat[15] = 0.0f;
    }
    return mat;
}


