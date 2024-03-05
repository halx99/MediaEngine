// #4
#include <glad/gl.h>
#include <GLFW/glfw3.h>
// #include <stb_image.h>
#include <assert.h>
#include <learnopengl/filesystem.h>
#include <learnopengl/shader_s.h>

#include <iostream>
#include <format>

#define AX_GLES_PROFILE 300
#define AX_GLES_PROFILE_DEN 100

#include "media/MediaEngine.h"

#include "glm/glm.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

/*
 * --silent --err-format=msvc --lang=glsl --profile=330 --include-dirs=/home/vmroot/dev/axmol/core/renderer/shaders
 * --flatten-ubos --frag=/home/vmroot/dev/axmol/core/renderer/shaders/colorNormalTexture.frag
 * --output=/home/vmroot/dev/axmol/build/runtime/axslc/colorNormalTexture
 */

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

#define CHECK_GL_ERROR_DEBUG()                                                                      \
    do                                                                                              \
    {                                                                                               \
        GLenum __error = glGetError();                                                              \
        if (__error)                                                                                \
        {                                                                                           \
            printf("OpenGL error 0x%04X in %s %s %d\n", __error, __FILE__, __FUNCTION__, __LINE__); \
        }                                                                                           \
    } while (false)
#define CHECK_GL_ERROR_ABORT()                                                                      \
    do                                                                                              \
    {                                                                                               \
        GLenum __error = glGetError();                                                              \
        if (__error)                                                                                \
        {                                                                                           \
            printf("OpenGL error 0x%04X in %s %s %d\n", __error, __FILE__, __FUNCTION__, __LINE__); \
            assert(false);                                                                          \
        }                                                                                           \
    } while (false)

// settings
const unsigned int SCR_WIDTH  = 1024;
const unsigned int SCR_HEIGHT = 768;

float designWidth  = 1988;
float desighHeight = 1200;

static void recreate_texture(GLuint& texture, GLint samplerFilter = GL_LINEAR, GLint wrap = GL_CLAMP_TO_EDGE)
{
    if (texture)
        glDeleteTextures(1, &texture);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D,
                  texture);  // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    wrap);  // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, samplerFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, samplerFilter);
}

struct V3F_C4B_T2F
{
    /// vertices (3F)
    glm::vec3 vertices;  // 12 bytes

    /// colors (4B)
    unsigned char colors[4];  // 4 bytes

    // tex coords (2F)
    glm::vec2 texCoords;  // 8 bytes
};

/** @struct GLContextAttrs
 *
 * There are six opengl Context Attrs.
 */
struct GLContextAttrs
{
    int redBits;
    int greenBits;
    int blueBits;
    int alphaBits;
    int depthBits;
    int stencilBits;
    int multisamplingCount;
    bool visible   = true;
    bool decorated = true;
    bool vsync     = false;
#if defined(_WIN32)
    void* viewParent = nullptr;
#endif
};

int main()
{
    using namespace ax;
    static std::unique_ptr<MediaEngineFactory> meFactory = MediaEngineFactory::create();
    auto mEngine                                         = meFactory->createMediaEngine();

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
#if AX_GLES_PROFILE
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, AX_GLES_PROFILE / AX_GLES_PROFILE_DEN);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#elif defined(AX_USE_GL)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);  // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // We don't want the old OpenGL
#endif

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    bool resizable                 = false;
    GLContextAttrs _glContextAttrs = {8, 8, 8, 8, 24, 8, 0};

    glfwWindowHint(GLFW_RESIZABLE, resizable ? GL_TRUE : GL_FALSE);
    glfwWindowHint(GLFW_RED_BITS, _glContextAttrs.redBits);
    glfwWindowHint(GLFW_GREEN_BITS, _glContextAttrs.greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, _glContextAttrs.blueBits);
    glfwWindowHint(GLFW_ALPHA_BITS, _glContextAttrs.alphaBits);
    glfwWindowHint(GLFW_DEPTH_BITS, _glContextAttrs.depthBits);
    glfwWindowHint(GLFW_STENCIL_BITS, _glContextAttrs.stencilBits);

    glfwWindowHint(GLFW_SAMPLES, _glContextAttrs.multisamplingCount);

    glfwWindowHint(GLFW_VISIBLE, _glContextAttrs.visible);
    glfwWindowHint(GLFW_DECORATED, _glContextAttrs.decorated);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HelloCpp-Emulator", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    // if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    //{
    //    std::cout << "Failed to initialize GLAD" << std::endl;
    //    return -1;
    //}
#if !defined(AX_GLES_PROFILE)
    if (!gladLoadGL(glfwGetProcAddress))
    {
        printf("glad: Failed to Load GL");
        return false;
    }
#else
    if (!gladLoadGLES2(glfwGetProcAddress))
    {
        printf("glad: Failed to Load GLES2");
        return false;
    }
#endif

    auto vendor   = (char const*)glGetString(GL_VENDOR);
    auto renderer = (char const*)glGetString(GL_RENDERER);
    auto version  = (char const*)glGetString(GL_VERSION);

    printf(
        "###########################\nDriver "
        "info:\nvender:%s\nrenderer:%s\nversion:%s\n###########################\n\n",
        vendor, renderer, version);

    framebuffer_size_callback(window, 1024, 768);

    // build and compile our shader zprogram
    // ------------------------------------
    auto vertSourcePath       = FileSystem::getPath("Content/sprite.vert");
    auto spriteFragSourcePath = FileSystem::getPath("Content/sprite.frag");
    Shader ourShader(vertSourcePath.c_str(), spriteFragSourcePath.c_str());

    float floatStride = sizeof(V3F_C4B_T2F) / 4;

    // ------------------------------------------------------------------

#if 1
    V3F_C4B_T2F vertices[] = {{glm::vec3{862.0f, 728.0f, 0.0f}, {255, 255, 255, 255}, {0.0f, 0.0f}},
                              {glm::vec3{862.0f, 472.0f, 0.0f}, {255, 255, 255, 255}, {0.0f, 1.0f}},
                              {glm::vec3{1118.0f, 728.0f, 0.0f}, {255, 255, 255, 255}, {1.0f, 0.0f}},
                              {glm::vec3{1118.0f, 472.0f, 0.0f}, {255, 255, 255, 255}, {1.0f, 1.0f}}};

#else
    float vertices[] = {
        // positions          // colors           // texture coords
        862.0f,  728.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,  // left bottom
        862.0f,  472.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,  // bottom right
        1118.0f, 728.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,  // bottom top
        1118.0f, 472.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f   // top left
    };
#endif
    unsigned short indices[] = {
        0, 1, 2,  // first triangle
        3, 2, 1   // second triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    // load and create a texture
    // -------------------------
    GLuint texture1{0};
    int width, height, nrChannels;
    recreate_texture(texture1);
    // load image, create texture and generate mipmaps
    // stbi_set_flip_vertically_on_load(true);  // tell stb_image.h to flip loaded texture's on the y-axis.
    auto data = stbi_load(FileSystem::getPath("Content/logo.png").c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the
        // data type is of GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        // glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    ourShader.use();  // don't forget to activate/use the shader before setting uniforms!
    // either set it manually like so:
    //
    auto texLoc = glGetUniformLocation(ourShader.ID, "texture1");
    // glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0);
    // or set it via the texture class
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // ourShader.setInt("texture1", 0);

    // enable alphablend
    glEnable(GL_BLEND);
    glBlendEquationSeparate(32774, 32774);
    glBlendFuncSeparate(1, 771, 1, 771);
    glColorMask(1, 1, 1, 1);

#if 0
    /* !!!!!!enable stencil test will cause intel UHD 630 Graphics d3d11 driver viewport bug.
     when viewport x or y < 0, that's why modern grapics libraries doesn't allow set negative viewport
     i.e
       - dawn: google wgpu implementation 
       - bgfx
     */
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 0, 0);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glStencilMask(0);
#endif

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClearDepthf(1.00f);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_ALWAYS);
        glClearStencil(0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // glClear(GL_COLOR_BUFFER_BIT);

        glDisable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glClearDepthf(1.00f);

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

        // color attribute
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, floatStride * sizeof(float), (void*)(3 * sizeof(float)));

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, floatStride * sizeof(float), (void*)0);

        // texture coord attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, floatStride * sizeof(float), (void*)(4 * sizeof(float)));

        // bind textures on corresponding texture units
        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(GL_TEXTURE_2D, texture2);

        // render container
        // ourShader.use();
        // glBindVertexArray(VAO);
        glUniform1i(texLoc, 0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(-121, 0, 1267 /*width*/, height);
}
