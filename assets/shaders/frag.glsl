#version 330

out vec4 outColor;
in vec4 fragPos;

void main()
{
	outColor = vec4(0.0, 1.0, 0.0, 1.0);
	
	if(fract(abs(fragPos.z)) == 0.0) 
	{
		outColor.x *= 0.9;
		outColor.y *= 0.9;
		outColor.z *= 0.9;
	} 
	else if(fract(abs(fragPos.y)) == 0.0) 
	{
		outColor.x *= 0.8;
		outColor.y *= 0.8;
		outColor.z *= 0.8;
	}
}
