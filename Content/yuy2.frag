#version 450
layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec3 ourColor;
layout(location = 1) in vec2 TexCoord;

layout(binding = 0) uniform sampler2D lumaTexture; // Y sample
layout(binding = 1) uniform sampler2D chromaTexture; // UV sample

// 'non-opaque uniforms outside a block' : not allowed when using GLSL for Vulkan
layout(std140, binding = 0) uniform UBO {
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
