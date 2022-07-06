// Basic Texture Shader

#type vertex
#version 450 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColour;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in float aTexIndex;
layout(location = 4) in float aTilingFactor;
layout(location = 5) in int aEntityID;

layout(std140, binding = 0) uniform Camera
{
	mat4 uViewProjection;
};

struct VertexOutput
{
	vec4 Colour;
	vec2 TexCoord;
	float TilingFactor;
};

layout (location = 0) out VertexOutput Output;
layout (location = 3) out flat float vTexIndex;
layout (location = 4) out flat int vEntityID;

void main()
{
	Output.Colour = aColour;
	Output.TexCoord = aTexCoord;
	Output.TilingFactor = aTilingFactor;
	vTexIndex = aTexIndex;
	vEntityID = aEntityID;

	gl_Position = uViewProjection * vec4(aPosition, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 oColour;
layout(location = 1) out int oEntityID;

struct VertexOutput
{
	vec4 Colour;
	vec2 TexCoord;
	float TilingFactor;
};

layout (location = 0) in VertexOutput Input;
layout (location = 3) in flat float vTexIndex;
layout (location = 4) in flat int vEntityID;

layout (binding = 0) uniform sampler2D uTextures[32];

void main()
{
	vec4 texColour = Input.Colour;

	switch(int(vTexIndex))
	{
		case  0: texColour *= texture(uTextures[ 0], Input.TexCoord * Input.TilingFactor); break;
		case  1: texColour *= texture(uTextures[ 1], Input.TexCoord * Input.TilingFactor); break;
		case  2: texColour *= texture(uTextures[ 2], Input.TexCoord * Input.TilingFactor); break;
		case  3: texColour *= texture(uTextures[ 3], Input.TexCoord * Input.TilingFactor); break;
		case  4: texColour *= texture(uTextures[ 4], Input.TexCoord * Input.TilingFactor); break;
		case  5: texColour *= texture(uTextures[ 5], Input.TexCoord * Input.TilingFactor); break;
		case  6: texColour *= texture(uTextures[ 6], Input.TexCoord * Input.TilingFactor); break;
		case  7: texColour *= texture(uTextures[ 7], Input.TexCoord * Input.TilingFactor); break;
		case  8: texColour *= texture(uTextures[ 8], Input.TexCoord * Input.TilingFactor); break;
		case  9: texColour *= texture(uTextures[ 9], Input.TexCoord * Input.TilingFactor); break;
		case 10: texColour *= texture(uTextures[10], Input.TexCoord * Input.TilingFactor); break;
		case 11: texColour *= texture(uTextures[11], Input.TexCoord * Input.TilingFactor); break;
		case 12: texColour *= texture(uTextures[12], Input.TexCoord * Input.TilingFactor); break;
		case 13: texColour *= texture(uTextures[13], Input.TexCoord * Input.TilingFactor); break;
		case 14: texColour *= texture(uTextures[14], Input.TexCoord * Input.TilingFactor); break;
		case 15: texColour *= texture(uTextures[15], Input.TexCoord * Input.TilingFactor); break;
		case 16: texColour *= texture(uTextures[16], Input.TexCoord * Input.TilingFactor); break;
		case 17: texColour *= texture(uTextures[17], Input.TexCoord * Input.TilingFactor); break;
		case 18: texColour *= texture(uTextures[18], Input.TexCoord * Input.TilingFactor); break;
		case 19: texColour *= texture(uTextures[19], Input.TexCoord * Input.TilingFactor); break;
		case 20: texColour *= texture(uTextures[20], Input.TexCoord * Input.TilingFactor); break;
		case 21: texColour *= texture(uTextures[21], Input.TexCoord * Input.TilingFactor); break;
		case 22: texColour *= texture(uTextures[22], Input.TexCoord * Input.TilingFactor); break;
		case 23: texColour *= texture(uTextures[23], Input.TexCoord * Input.TilingFactor); break;
		case 24: texColour *= texture(uTextures[24], Input.TexCoord * Input.TilingFactor); break;
		case 25: texColour *= texture(uTextures[25], Input.TexCoord * Input.TilingFactor); break;
		case 26: texColour *= texture(uTextures[26], Input.TexCoord * Input.TilingFactor); break;
		case 27: texColour *= texture(uTextures[27], Input.TexCoord * Input.TilingFactor); break;
		case 28: texColour *= texture(uTextures[28], Input.TexCoord * Input.TilingFactor); break;
		case 29: texColour *= texture(uTextures[29], Input.TexCoord * Input.TilingFactor); break;
		case 30: texColour *= texture(uTextures[30], Input.TexCoord * Input.TilingFactor); break;
		case 31: texColour *= texture(uTextures[31], Input.TexCoord * Input.TilingFactor); break;
	}

	if (texColour.a == 0.0)
		discard;

	oColour = texColour;
	oEntityID = vEntityID;
}
