# Play with OpenGL (GLFW)
## Projects
- basic_01_GLFW_only
- basic_02_shader_cube_control
- basic_03_object

## How to build a project
```
cd play_with_opengl
git submodule init
git submodule update

cd basic_01_GLFW_only
mkdir build && cd build
cmake ..
make
```

## Acknowledgements
### External libraries
- third_party/fw
	- https://github.com/glfw/glfw.git
	- as git submodule (version is 3.3.2)
- third_party/glew
	- https://sourceforge.net/projects/glew/files/glew/2.1.0/glew-2.1.0.zip/download
- third_party/glm
	- https://github.com/g-truc/glm.git
	- as git submodule (version is 0.9.9.8)

### Code
- Some code is retrieved from:
	- https://www.opengl-tutorial.org/

