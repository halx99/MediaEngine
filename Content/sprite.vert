#version 300 es
layout(location = 0) in vec4 aPos;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec4 ourColor;

mat4 mvpTest = mat4(
    1.0497278f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.73205090f, 0.0f, 0.0f,
    0.0f, 0.0f, -1.00061023, -1.0f,
    -1039.23047f, -1039.23059f, 1038.86438f, 1039.23047f
);

void main()
{
	gl_Position = mvpTest * aPos; //vec4(aPos, 1);
    ourColor = aColor;
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}
