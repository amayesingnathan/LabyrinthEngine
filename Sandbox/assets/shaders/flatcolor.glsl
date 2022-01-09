// Basic Colour Shader

#type vertex
#version 330 core

layout(location = 0) in vec3 aPosition;

uniform mat4 uViewProjection;
uniform mat4 uTransform;

void main()	
{
	gl_Position = uViewProjection * uTransform * vec4(aPosition, 1.0);	
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;

uniform vec4 uColor;

void main()
{
	color = uColor;
}