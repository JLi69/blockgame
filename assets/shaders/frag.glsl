#version 330

out vec4 outColor;
in vec4 fragPos;
in vec2 tc;
uniform sampler2D blockTextures;

void main()
{
	outColor = texture(blockTextures, tc);
	
	if(fract(abs(fragPos.x)) == 0.0) 
	{
		outColor.r *= 0.9;
		outColor.g *= 0.9;
		outColor.b *= 0.9;
	} 
	else if(fract(abs(fragPos.z)) == 0.0) 
	{
		outColor.r *= 0.8;
		outColor.g *= 0.8;
		outColor.b *= 0.8;
	}
}
