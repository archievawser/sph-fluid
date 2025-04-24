#version 330 core

in vec2 texCoord;
uniform sampler2D colorTex;
out vec4 FragColor;

void main()
{
	vec4 preAlpha = texture(colorTex, texCoord);
	FragColor = preAlpha * preAlpha.w;
}