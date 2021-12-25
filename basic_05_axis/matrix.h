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
#ifndef MATRIX_H
#define MATRIX_H

/*** Include ***/
/* for general */
#include <cstdint>
#include <cstdio>
#include <stdexcept>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>

class Matrix
{
public:
    Matrix()
        : m_rows(0), m_cols(0) {}
    Matrix(int32_t rows, int32_t cols)
    {
        m_rows = rows;
        m_cols = cols;
        m_data_array = std::vector<float>(m_rows * m_cols);
    }
    Matrix(int32_t rows, int32_t cols, float* data)
    {
        m_rows = rows;
        m_cols = cols;
        m_data_array = std::vector<float>(m_rows * m_cols);
        std::copy(data, data + m_rows * m_cols, m_data_array.data());
    }
    Matrix(int32_t rows, int32_t cols, const std::vector<float>& data)
    {
        m_rows = rows;
        m_cols = cols;
        m_data_array = data;
    }
    ~Matrix() {}
    const float* Data() const
    {
        return m_data_array.data();
    }
    float* Data()
    {
        return &m_data_array[0];
    }
    const float& operator[](int32_t i) const
    {
        return m_data_array.at(i);
    }

    float& operator[](int32_t i)
    {
        return m_data_array.at(i);
    }
    float& operator() (int32_t row, int32_t col)
    {
        if (row >= m_rows || col >= m_cols) throw std::out_of_range("Invalid index");
        return m_data_array.at(row * m_cols + col);
    }

    const float& operator() (int32_t row, int32_t col) const
    {
        if (row >= m_rows || col >= m_cols) throw std::out_of_range("Invalid index");
        return m_data_array.at(row * m_cols + col);
    }

    Matrix operator+(const Matrix& right) const
    {
        const Matrix& left = *this;
        if ((left.m_rows != right.m_rows) || (left.m_cols != right.m_cols)) throw std::out_of_range("Invalid index");
        Matrix ret(left.m_rows, right.m_cols);
        for (int32_t i = 0; i < m_rows * m_cols; i++) {
            ret[i] = left[i] + right[i];
        }
        return ret;
    }
    Matrix operator-(const Matrix& right) const
    {
        const Matrix& left = *this;
        if ((left.m_rows != right.m_rows) || (left.m_cols != right.m_cols)) throw std::out_of_range("Invalid index");
        Matrix ret(left.m_rows, right.m_cols);
        for (int32_t i = 0; i < m_rows * m_cols; i++) {
            ret[i] = left[i] - right[i];
        }
        return ret;
    }
    Matrix operator*(const float& k) const
    {
        const Matrix& left = *this;
        Matrix ret(left.m_rows, left.m_cols);
        for (int32_t i = 0; i < m_rows * m_cols; i++) {
            ret[i] = left[i] * k;
        }
        return ret;
    }
    Matrix operator*(const Matrix& right) const
    {
        const Matrix& left = *this;
        if (left.m_cols != right.m_rows) throw std::out_of_range("Invalid index");
        Matrix ret(left.m_rows, right.m_cols);
        for (int32_t row = 0; row < left.m_rows; row++) {
            for (int32_t col = 0; col < right.m_cols; col++) {
                ret(row, col) = 0.0f;
                for (int32_t i = 0; i < left.m_cols; i++) {
                    ret(row, col) += left(row, i) * right(i, col);
                }
            }
        }
        return ret;
    }
    Matrix Transpose() const
    {
        const Matrix& mat = *this;
        Matrix ret(mat.m_cols, mat.m_rows);
        for (int32_t row = 0; row < m_rows; row++) {
            for (int32_t col = 0; col < m_cols; col++) {
                ret(col, row) = mat(row, col);
            }
        }
        return ret;
    }

    Matrix Inverse() const
    {
        if (m_rows != m_cols) {
            throw std::out_of_range("Invalid shape");
        }

        Matrix mat = *this;
        int32_t n = mat.m_rows;
        Matrix ret = Identity(n);

        for (int32_t y = 0; y < n; y++) {
            if (mat(y, y) == 0) {
                throw std::out_of_range("Tried to calculate an inverse of non - singular matrix");
            }
            float scale_to_1 = 1.0f / mat(y, y);
            for (int32_t x = 0; x < n; x++) {
                mat(y, x) *= scale_to_1;
                ret(y, x) *= scale_to_1;
            }
            for (int32_t yy = 0; yy < n; yy++) {
                if (yy != y) {
                    float scale_to_0 = mat(yy, y);
                    for (int32_t x = 0; x < n; x++) {
                        mat(yy, x) -= mat(y, x) * scale_to_0;
                        ret(yy, x) -= ret(y, x) * scale_to_0;
                    }
                }
            }
        }

        return ret;
    }

    void Print() const
    {
        if (m_rows == 1) {
            for (int32_t x = 0; x < m_cols; x++) {
                printf("%4.f ", (*this)(0, x));
            }
        } else if (m_cols == 1) {
            for (int32_t y = 0; y < m_rows; y++) {
                printf("%4.f ", (*this)(y, 0));
            }
        } else {
            for (int32_t y = 0; y < m_rows; y++) {
                for (int32_t x = 0; x < m_cols; x++) {
                    printf("%4.f ", (*this)(y, x));
                }
                printf("\n");
            }
        }
        printf("\n");
    }

    static Matrix Identity(int32_t size)
    {
        Matrix ret(size, size);
        std::fill(ret.Data(), ret.Data() + size * size, 0.0f);
        for (int32_t i = 0; i < size; i++) {
            ret(i, i) = 1.0f;
        }
        return ret;
    }

    static void Test()
    {
        try {
            Matrix mat1(2, 3, { 1, 2, 3, 4, 5, 6 });
            Matrix mat2(2, 3, { 7, 8, 9, 10, 11, 12 });
            Matrix mat3(3, 2, { 1, 2, 3, 4, 5, 6 });
            Matrix mat4(2, 2, { 1, 2, 3, 4 });
            Matrix mat5(3, 3, { 2, 2, 3, 4, 5, 6, 7, 8, 9 });
            Matrix mat6(3, 3, { 1, 2, 3, 4, 5, 6, 7, 8, 9 });

            printf("\n--- mat1 ---\n");
            mat1.Print();

            printf("\n--- mat2 ---\n");
            mat2.Print();

            printf("\n--- add ---\n");
            Matrix matAdd = mat1 + mat2;
            matAdd.Print();

            printf("\n--- sub ---\n");
            Matrix matSub = mat1 - mat2;
            matSub.Print();

            printf("\n--- scalar ---\n");
            Matrix mat_k = mat1 * 2.0f;
            mat_k.Print();

            printf("\n--- mul ---\n");
            Matrix matMul = mat1 * mat3;
            matMul.Print();

            printf("\n--- transpose ---\n");
            Matrix matTranspose = mat1.Transpose();
            matTranspose.Print();

            printf("\n--- Identity matrix ---\n");
            Matrix matI = Matrix::Identity(3);
            matI.Print();

            printf("\n--- Inverse matrix 2x2 ---\n");
            Matrix matInv = mat4.Inverse();
            matInv.Print();
            (mat4 * matInv).Print();
            (matInv * mat4).Print();

            printf("\n--- Inverse matrix 3x3 ---\n");
            matInv = mat5.Inverse();
            matInv.Print();
            (mat5 * matInv).Print();
            (matInv * mat5).Print();

            printf("\n--- Inverse of non-singular matrix 3x3 ---\n");
            matInv = mat6.Inverse();
        }
        catch (std::exception e) {
            printf("Exception: %s\n", e.what());
        }
    }

private:
    std::vector<float> m_data_array;
    int32_t m_rows;
    int32_t m_cols;
};

#endif
