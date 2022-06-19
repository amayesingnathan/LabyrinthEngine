//
// Renderer 2D Circle Shader
//

#type vertex
#version 450 core

layout(location = 0) in vec3 aWorldPosition;
layout(location = 1) in float aThickness;
layout(location = 2) in vec2 aLocalPosition;
layout(location = 3) in vec4 aColour;
layout(location = 4) in int aEntityID;

layout (std140, binding = 0) uniform Camera
{
	mat4 uViewProjection;
};

struct VertexOutput
{
	vec2 LocalPosition;
	float Thickness;
	vec4 Colour;
};

layout (location = 0) out VertexOutput Output;
layout (location = 3) out flat int vEntityID;

void main()
{
	Output.LocalPosition = aLocalPosition;
	Output.Thickness = aThickness;
	Output.Colour = aColour;
	vEntityID = aEntityID;
	gl_Position = uViewProjection * vec4(aWorldPosition, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 oColour;
layout(location = 1) out int oEntityID;

struct VertexOutput
{
	vec2 LocalPosition;
	float Thickness;
	vec4 Colour;
};

layout (location = 0) in VertexOutput Input;
layout (location = 3) in flat int vEntityID;

void main()
{
	float fade = 0.01;
	float dist = sqrt(dot(Input.LocalPosition, Input.LocalPosition));
	float t = Input.Thickness;
	if (dist > 1.0 || dist < 1.0 - t - fade)
		discard;

	float alpha = 1.0 - smoothstep(1.0f - fade, 1.0f, dist);
	alpha *= smoothstep(1.0 - t - fade, 1.0 - t, dist);

	if (alpha == 0.0)
		discard;

	oColour = Input.Colour;
	oColour.a = Input.Colour.a * alpha;

	oEntityID = vEntityID;
}
