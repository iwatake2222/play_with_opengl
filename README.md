# Play with OpenGL (GLFW)
## Projects
- basic_01_blank
- basic_02_shader_cube_control
- basic_03_object

## How to build a project
```
# sudo apt-get install libglu1-mesa-dev mesa-common-dev
cd play_with_opengl
git submodule init
git submodule update

cd basic_02_shader_cube_control
mkdir build && cd build
cmake ..
make
```

## Acknowledgements
### External libraries
- third_party/glfw
	- https://github.com/glfw/glfw.git
	- as git submodule (version is 3.3.2)
- third_party/glew
	- https://sourceforge.net/projects/glew/files/glew/2.1.0/glew-2.1.0.zip/download
- third_party/glm
	- https://github.com/g-truc/glm.git
	- as git submodule (version is 0.9.9.8)
- third_party/assimp
	- https://github.com/assimp/assimp.git
	- as git submodule (version is v5.0.1)

### Code
- Some code and materials are retrieved from:
	- https://www.opengl-tutorial.org/

### Model
- resource/miku_Ver17.02.pmx
	- https://piapro.jp/t/0Hwp
	- 初音ミク_muuubu_202005042213.zip
	- む～ぶさん
- nendomiku_ver3_00.pmx
	- https://piapro.jp/t/wyAD
	- ねんどミクさん_maebari_202005042218.zip
	- maebariさん
