# For GLEW
# add_definitions(-DBUILD_UTILS=off)
# add_definitions(-DGLEW_STATIC)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../glew/build/cmake glew)
target_link_libraries(${ProjectName} glew_s)
target_include_directories(${ProjectName} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../glew/include)
