#version 330

layout(location = 0) in vec4 pos;

uniform mat4 uPerspectiveViewMat;
uniform mat4 uTransformMat;

out vec4 fragPos;

void main()
{	
	gl_Position = uPerspectiveViewMat * uTransformMat * pos;
	fragPos = pos;
}
