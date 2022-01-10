// Basic Texture Shader

#type vertex
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColour;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in float aTexIndex;
layout(location = 4) in float aScalingFactor;

uniform mat4 uViewProjection;

out vec4 vColour;
out vec2 vTexCoord;
out float vTexIndex;
out float vScalingFactor;

void main()
{
	vColour = aColour;
	vTexCoord = aTexCoord;
	vTexIndex = aTexIndex;
	vScalingFactor = aScalingFactor;
	gl_Position = uViewProjection * vec4(aPosition, 1.0);
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec4 vColour;
in vec2 vTexCoord;
in float vTexIndex;
in float vScalingFactor;

uniform sampler2D uTextures[32];

void main()
{
	color = texture(uTextures[int(vTexIndex)], vTexCoord * vScalingFactor) * vColour;
}