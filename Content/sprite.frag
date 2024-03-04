#version 300 es
precision mediump float;
precision highp int;

layout(location = 0) out vec4 FragColor;

in vec2 TexCoord;

// texture sampler
uniform sampler2D texture1;

void main()
{
	FragColor = vec4(1.0, 1.0, 0.0, 1.0); //texture(texture1, TexCoord);
}
