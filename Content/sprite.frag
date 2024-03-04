#version 300 es
precision mediump float;
precision highp int;

layout(location = 0) out vec4 FragColor;

in vec2 TexCoord;
in vec4 ourColor;

// texture sampler
uniform sampler2D texture1;

void main()
{
	FragColor = ourColor * texture(texture1, TexCoord);
}
