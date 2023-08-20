#version 330 core

out vec4 outColor;
in vec4 fragPos;
in vec2 tc;
uniform sampler2D blockTextures;
uniform vec3 uSelected;

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
		outColor.r *= 0.7;
		outColor.g *= 0.7;
		outColor.b *= 0.7;
	}

	if(fragPos.x / 2.0 + 0.5 - uSelected.x <= 1.0 &&
	   fragPos.x / 2.0 + 0.5 - uSelected.x >= 0.0 &&
	   fragPos.y / 2.0 + 0.5 - uSelected.y <= 1.0 &&
	   fragPos.y / 2.0 + 0.5 - uSelected.y >= 0.0 &&
	   fragPos.z / 2.0 + 0.5 - uSelected.z <= 1.0 &&
	   fragPos.z / 2.0 + 0.5 - uSelected.z >= 0.0) {
		outColor = mix(outColor, vec4(1.0, 1.0, 1.0, 1.0), 0.4);
	}
}
