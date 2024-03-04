#version 300 es
precision mediump float;
precision highp int;

layout(location = 0) out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D lumaTexture; // Y sample
uniform sampler2D chromaTexture; // UV sample

// 'non-opaque uniforms outside a block' : not allowed when using GLSL for Vulkan
layout(std140) uniform fs_ub {
    mat4 colorTransform;
};

vec3 trasnformYUV(vec3 YUV)
{
    YUV -= vec3(colorTransform[0].w, colorTransform[1].w, colorTransform[2].w);
    return mat3(
        colorTransform[0].xyz,
        colorTransform[1].xyz,
        colorTransform[2].xyz
    ) * YUV;
}

void main()
{
    vec3 YUV;
    
    /* For dual sampler */
    YUV.x = texture(lumaTexture, TexCoord).x;
    YUV.yz = texture(chromaTexture, TexCoord).yw; // UV: 
	
    /* Convert YUV to RGB */
    vec4 OutColor;
    OutColor.xyz = trasnformYUV(YUV); // * ourColor;
    OutColor.w = 1.0;

    FragColor = OutColor;
}
