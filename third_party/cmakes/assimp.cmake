# For ASSIMP
if(MSVC)
	set(CMAKE_CXX_FLAGS "/wd4819 ${CMAKE_CXX_FLAGS}" CACHE STRING "" FORCE)
endif()
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

add_definitions(-DBUILD_SHARED_LIBS=off -DASSIMP_BUILD_ASSIMP_TOOLS=off -DASSIMP_BUILD_TESTS=off -DASSIMP_INSTALL=off)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../assimp assimp)
target_link_libraries(${ProjectName} assimp)
target_include_directories(${ProjectName} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../assimp/include)

