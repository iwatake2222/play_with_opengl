#ifndef SHADER_H
#define SHADER_H

GLuint CreateShaderProgram(const char* vertex_shader_text, const char* fragment_shader_text);
GLuint LoadShaderProgram(const char* vertex_shader_path, const char* fragment_shader_path);

#endif
