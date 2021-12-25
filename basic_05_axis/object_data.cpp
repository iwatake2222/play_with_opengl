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
/*** Include ***/
/* for general */
#include <cstdint>
#include <cstdio>
#include <fstream> 
#include <vector>
#include <string>
#include <memory>
#include <algorithm>

#include "shape.h"
#include "transform.h"
#include "object_data.h"

/*** Macro ***/
/* macro function */
#define RUN_CHECK(x)                                         \
  if (!(x)) {                                                \
    fprintf(stderr, "Error at %s:%d\n", __FILE__, __LINE__); \
    exit(1);                                                 \
  }

/* Setting */

/*** Global variable ***/


/*** Function ***/
Shape* CreateGround(float size, float interval, std::vector<float> color_vec3)
{
    GLuint index = 0;
    std::vector<Object::Vertex> vertex_list;
    std::vector<GLuint> index_list;
    for (float z = -size; z <= size; z += interval) {
        vertex_list.push_back({ -size, 0.0f, z, color_vec3[0], color_vec3[1], color_vec3[2] });
        vertex_list.push_back({ size, 0.0f, z, color_vec3[0], color_vec3[1], color_vec3[2] });
        index_list.push_back(index++);
        index_list.push_back(index++);
    }
    for (float x = -size; x <= size; x += interval) {
        vertex_list.push_back({ x, 0.0f, -size, color_vec3[0], color_vec3[1], color_vec3[2] });
        vertex_list.push_back({ x, 0.0f, size, color_vec3[0], color_vec3[1], color_vec3[2] });
        index_list.push_back(index++);
        index_list.push_back(index++);
    }
    return new ShapeIndex(vertex_list, index_list);
}
