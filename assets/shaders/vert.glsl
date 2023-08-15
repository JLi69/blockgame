#version 330 core

layout(location = 0) in vec4 pos;
layout(location = 1) in vec2 texCoord;

uniform mat4 uPerspectiveViewMat;
uniform mat4 uTransformMat;

out vec4 fragPos;
out vec2 tc;

void main()
{	
	gl_Position = uPerspectiveViewMat * uTransformMat * pos;
	fragPos = pos;
	tc = texCoord;
}
