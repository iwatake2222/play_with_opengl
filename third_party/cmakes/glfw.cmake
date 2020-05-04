# For OpenGL
# may need this (sudo apt-get install libglu1-mesa-dev  mesa-common-dev)
# find_package(OpenGL REQUIRED)
# target_link_libraries(${ProjectName} ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY})
# target_link_libraries(${ProjectName} ${OPENGL_LIBRARIES})
# target_include_directories(${ProjectName} PUBLIC ${OPENGL_INCLUDE_DIR})

# For GLFW
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../glfw glfw)
target_link_libraries(${ProjectName} glfw ${GLFW_LIBRARIES})
target_include_directories(${ProjectName} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../glfw/include)
# target_include_directories(${ProjectName} PUBLIC ${glfw_INCLUDE_DIRS})
