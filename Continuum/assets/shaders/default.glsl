// Default Framebuffer 

#type vertex
#version 450 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;

out vec2 vTexCoord;

void main()
{
	gl_Position = vec4(aPosition, 1.0);
	vTexCoord = aTexCoord;
}

#type fragment
#version 450 core

layout(location = 0) out vec4 color;

in vec2 vTexCoord;

uniform sampler2D screenTex;

void main()
{
	color = texture(screenTex, vTexCoord);
}