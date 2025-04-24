#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 yeah;

uniform mat4 mvp;

out vec2 texCoord;

void main()
{
	texCoord = yeah;
	gl_Position = mvp * vec4(position, 1.0);
}