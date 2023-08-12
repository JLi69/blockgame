#include "shader.h"
#include <fstream>
#include <sstream>
#include <iostream>

std::string readShaderFile(const char* path)
{
	std::ifstream shaderFile(path);

	if(!shaderFile)
	{
		std::cerr << "Failed to open " << path << '\n';
		return "";
	}

	std::string line;
	std::stringstream shaderFileContents;
	//Read file line by line and concatonate them together
	while(std::getline(shaderFile, line))
		shaderFileContents << line << '\n';
	shaderFile.close();
	return shaderFileContents.str();
}

unsigned int createShader(const char* path, GLenum shaderType)
{
	unsigned int shader = glCreateShader(shaderType);

	//Read shader file
	std::string shaderCode = readShaderFile(path);
	const char *shaderCodeBegin = shaderCode.c_str();
	const int len = shaderCode.size();
	glShaderSource(shader, 1, &shaderCodeBegin, &len);

	//Compile the shader
	glCompileShader(shader);

	int compileStatus;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
	if(compileStatus != 1) //Shader failed to compile
	{
		std::cerr << path << " failed to compile!\n";

		//Output any compiler errors
		char message[1024]; //For now, we will just assume all error
							//messages < 1024 character long
		int len;
		glGetShaderInfoLog(shader, 1023, &len, message);	
		message[len] = '\0';
		std::cerr << message << '\n';
	}

	return shader;
}

ShaderProgram::ShaderProgram(unsigned int vertex, unsigned int fragment)
{
	programid = glCreateProgram();
	
	glAttachShader(programid, vertex);
	glAttachShader(programid, fragment);
	glLinkProgram(programid);
	glValidateProgram(programid);
	
	//Check for linker errors
	int linkStatus;
	glGetProgramiv(programid, GL_LINK_STATUS, &linkStatus);
	if(linkStatus != 1) //Failed to link
	{
		//Output linker errors
		std::cerr << "Failed to link program!\n";

		char message[1024];
		int len;
		glGetProgramInfoLog(programid, 1023, &len, message);
		std::cerr << message << '\n';
	}

	glDetachShader(programid, vertex);
	glDetachShader(programid, fragment);
}

void ShaderProgram::use()
{
	glUseProgram(programid);
}

int ShaderProgram::getUniformLocation(const char *uniformName)
{
	if(uniformLocations.count(uniformName) == 0)
	{
		int location = glGetUniformLocation(programid, uniformName);
		uniformLocations[uniformName] = location;
		return location;
	}

	return uniformLocations[uniformName];
}
