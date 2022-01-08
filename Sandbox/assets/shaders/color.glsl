// Basic Colour Shader

#type vertex
#version 330 core

layout(location = 0) in vec3 aPosition;

out vec3 vPosition;

uniform mat4 uViewProjection;
uniform mat4 uTransform;

void main()	
{
	vPosition = aPosition;
	gl_Position = uViewProjection * uTransform * vec4(aPosition, 1.0);	
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec3 vPosition;

uniform vec3 uColor;

void main()
{
	color = vec4(uColor, 1.0);
}