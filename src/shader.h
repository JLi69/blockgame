#ifndef __SHADER_H___
#include <string>
#include <glad/glad.h>

//Helper function that reads contents of a shader file, 
//returns the string containing the content,
//takes path of shader as argument
std::string readShaderFile(const char *path);
//Helper function that compiles a shader
//Takes a path of the shader file,
//also takes an argument for the type of shader
//reads the contents, then compiles the shader
//and returns the id of the shader
//will output any compiler errors to stderr
unsigned int createShader(const char *path, GLenum shaderType);
//Helper function that creates a shader
//program by taking in two file paths,
//one to the vertex shader and the other to
//the fragment shader, will output any compiler
//or linker errors to stderr, will return the
//program id
unsigned int createProgram(unsigned int vertex, unsigned int fragment);

#endif

#define __SHADER_H__
