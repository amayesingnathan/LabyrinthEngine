// Basic Line Shader

#type vertex
#version 450 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColour;
layout(location = 2) in int aEntityID;

layout(std140, binding = 0) uniform Camera
{
	mat4 uViewProjection;
};

struct VertexOutput
{
	vec4 Colour;
};

layout (location = 0) out VertexOutput Output;
layout (location = 1) out flat int vEntityID;

void main()
{
	Output.Colour = aColour;
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
};

layout (location = 0) in VertexOutput Input;
layout (location = 1) in flat int vEntityID;

void main()
{
	oColour = Input.Colour;
	oEntityID = vEntityID;
}
