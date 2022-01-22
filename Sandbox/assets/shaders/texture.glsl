// Basic Texture Shader

#type vertex
#version 450 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColour;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in float aTexIndex;
layout(location = 4) in float aTilingFactor;

uniform mat4 uViewProjection;

out vec4 vColour;
out vec2 vTexCoord;
out flat float vTexIndex;
out float vTilingFactor;

void main()
{
	vColour = aColour;
	vTexCoord = aTexCoord;
	vTexIndex = aTexIndex;
	vTilingFactor = aTilingFactor;
	gl_Position = uViewProjection * vec4(aPosition, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 color;

in vec4 vColour;
in vec2 vTexCoord;
in flat float vTexIndex;
in float vTilingFactor;

uniform sampler2D uTextures[32];

void main()
{
	vec4 texColour = vColour;
	switch(int(vTexIndex))
	{
		case 0: texColour *= texture(uTextures[0], vTexCoord * vTilingFactor); break;
		case 1: texColour *= texture(uTextures[1], vTexCoord * vTilingFactor); break;
		case 2: texColour *= texture(uTextures[2], vTexCoord * vTilingFactor); break;
		case 3: texColour *= texture(uTextures[3], vTexCoord * vTilingFactor); break;
		case 4: texColour *= texture(uTextures[4], vTexCoord * vTilingFactor); break;
		case 5: texColour *= texture(uTextures[5], vTexCoord * vTilingFactor); break;
		case 6: texColour *= texture(uTextures[6], vTexCoord * vTilingFactor); break;
		case 7: texColour *= texture(uTextures[7], vTexCoord * vTilingFactor); break;
		case 8: texColour *= texture(uTextures[8], vTexCoord * vTilingFactor); break;
		case 9: texColour *= texture(uTextures[9], vTexCoord * vTilingFactor); break;
		case 10: texColour *= texture(uTextures[10], vTexCoord * vTilingFactor); break;
		case 11: texColour *= texture(uTextures[11], vTexCoord * vTilingFactor); break;
		case 12: texColour *= texture(uTextures[12], vTexCoord * vTilingFactor); break;
		case 13: texColour *= texture(uTextures[13], vTexCoord * vTilingFactor); break;
		case 14: texColour *= texture(uTextures[14], vTexCoord * vTilingFactor); break;
		case 15: texColour *= texture(uTextures[15], vTexCoord * vTilingFactor); break;
		case 16: texColour *= texture(uTextures[16], vTexCoord * vTilingFactor); break;
		case 17: texColour *= texture(uTextures[17], vTexCoord * vTilingFactor); break;
		case 18: texColour *= texture(uTextures[18], vTexCoord * vTilingFactor); break;
		case 19: texColour *= texture(uTextures[19], vTexCoord * vTilingFactor); break;
		case 20: texColour *= texture(uTextures[20], vTexCoord * vTilingFactor); break;
		case 21: texColour *= texture(uTextures[21], vTexCoord * vTilingFactor); break;
		case 22: texColour *= texture(uTextures[22], vTexCoord * vTilingFactor); break;
		case 23: texColour *= texture(uTextures[23], vTexCoord * vTilingFactor); break;
		case 24: texColour *= texture(uTextures[24], vTexCoord * vTilingFactor); break;
		case 25: texColour *= texture(uTextures[25], vTexCoord * vTilingFactor); break;
		case 26: texColour *= texture(uTextures[26], vTexCoord * vTilingFactor); break;
		case 27: texColour *= texture(uTextures[27], vTexCoord * vTilingFactor); break;
		case 28: texColour *= texture(uTextures[28], vTexCoord * vTilingFactor); break;
		case 29: texColour *= texture(uTextures[29], vTexCoord * vTilingFactor); break;
		case 30: texColour *= texture(uTextures[30], vTexCoord * vTilingFactor); break;
		case 31: texColour *= texture(uTextures[31], vTexCoord * vTilingFactor); break;
	}
	color = texColour;
}