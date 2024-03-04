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

/*
* --silent --err-format=msvc --lang=glsl --profile=330 --include-dirs=/home/vmroot/dev/axmol/core/renderer/shaders --flatten-ubos --frag=/home/vmroot/dev/axmol/core/renderer/shaders/colorNormalTexture.frag --output=/home/vmroot/dev/axmol/build/runtime/axslc/colorNormalTexture
*/

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

#define CHECK_GL_ERROR_DEBUG()                                                                       \
    do                                                                                               \
    {                                                                                                \
        GLenum __error = glGetError();                                                               \
        if (__error)                                                                                 \
        {                                                                                            \
            printf("OpenGL error 0x%04X in %s %s %d\n", __error, __FILE__, __FUNCTION__, __LINE__); \
        }                                                                                            \
    } while (false)
#define CHECK_GL_ERROR_ABORT()                                                                       \
    do                                                                                               \
    {                                                                                                \
        GLenum __error = glGetError();                                                               \
        if (__error)                                                                                 \
        {                                                                                            \
            printf("OpenGL error 0x%04X in %s %s %d\n", __error, __FILE__, __FUNCTION__, __LINE__); \
            assert(false);                                                                           \
        }                                                                                            \
    } while (false)


// settings
const unsigned int SCR_WIDTH  = 1024;
const unsigned int SCR_HEIGHT = 768;

float designWidth = 1988;
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

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "axplay", NULL, NULL);
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

    printf("###########################\nDriver info:\nvender:%s\nrenderer:%s\nversion:%s\n###########################\n\n", vendor, renderer, version);

    framebuffer_size_callback(window, 1024, 768);

    // build and compile our shader zprogram
    // ------------------------------------
    auto vertSourcePath = FileSystem::getPath("Content/sprite.vert");
    auto fragSourcePath = FileSystem::getPath("Content/yuy2.frag");
    Shader ourShader(vertSourcePath.c_str(), fragSourcePath.c_str());

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions        // texture coords
        0.5f,  0.5f, 0.0f, 1.0f, 1.0f,  // top right
        0.5f,  -0.5f,0.0f, 1.0f, 0.0f,  // bottom right
        -0.5f, -0.5f,0.0f, 0.0f, 0.0f,  // bottom left
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f   // top left
    };
    unsigned int indices[] = {
        0, 1, 3,  // first triangle
        1, 2, 3   // second triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    MEVideoPixelDesc vpd;

    // TODO: update textureRect by vertex data
    bool bFullColorRange = true;
    const Mat4 colorTransform = bFullColorRange ? Mat4{ // 709Scaled
            1.16438356f,   0.00000000f,    1.79265225f,     0.0f,
            1.16438356f,  -0.213237017f, - 0.533004045f,    0.0f,
            1.16438356f,   2.11241937f,    0.00000000f,     0.0f,
            0.0627451017f, 0.501960814f,   0.501960814f,    0.0f // YUVOffset8Bits: 16/255.0f, 128/255.0f, 128/255.0f
        } : Mat4 { // 709Unscaled
            1.000000f,  0.0000000f,       1.57472198f,      0.0f,
		    1.000000f, -0.187314089f,     -0.46820747f,     0.0f,
		    1.000000f,  1.85561536f,      0.0000000f,       0.0f,
		    0.0627451f, 0.5019608f,       0.50196081f,      0.0f
        };

    // allocate ubo to store colorTransform
    GLuint ubo = 0;
    glGenBuffers(1, &ubo); // gen ubo
    glBindBuffer(GL_UNIFORM_BUFFER, ubo); // bind ubo
    glBufferData(GL_UNIFORM_BUFFER, sizeof(colorTransform), NULL, GL_STATIC_DRAW); // update ubo data
    auto pbuf = glMapBufferOES(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    memcpy(pbuf, &colorTransform, sizeof(colorTransform));
    glUnmapBuffer(GL_UNIFORM_BUFFER);
    glBindBuffer(GL_UNIFORM_BUFFER, 0); // unbind

    auto videoUri = FileSystem::getPath("Content/video/h264/1280x720.mp4");
    videoUri.insert(0, "file:///");

    mEngine->setAutoPlay(true);
    mEngine->open(videoUri);
    mEngine->setLoop(true);

    GLuint ySample = 0, uvSample = 0;
    mEngine->setCallbacks([](MEMediaEventType event) { printf("Media Event: %d\n", (int)event); },
                          [&](const MEVideoFrame& frame) {
        ourShader.use();

        CHECK_GL_ERROR_DEBUG();

        bool bPixelDescChnaged = !vpd.equals(frame._vpd);
        if (bPixelDescChnaged)
        {  // recreate texture
            recreate_texture(ySample);
            recreate_texture(uvSample);
            CHECK_GL_ERROR_DEBUG();
        }
        vpd = frame._vpd;

        // upload texture
        assert(vpd._PF == MEVideoPixelFormat::YUY2);  // Now only render YUY2

        auto pixelFormat = vpd._PF;

        // (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum
        // format, GLenum type, const void * pixels);
        auto& bufferDim = frame._vpd._dim;


        /* ------------ LumaTexture aka Y ------------- */
        // Bind our texture in Texture Unit 0 for LumaTexture
        glActiveTexture(GL_TEXTURE0);
        CHECK_GL_ERROR_ABORT();
        glBindTexture(GL_TEXTURE_2D, ySample);
        CHECK_GL_ERROR_ABORT();
        // OpenGL 330 core profile doesn't support LA8, support RG8
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RG8, bufferDim.x, bufferDim.y, 0, GL_RG,
                     GL_UNSIGNED_BYTE, frame._dataPointer);
        CHECK_GL_ERROR_ABORT();
        // Set our "lumaTexture" sampler to use Texture Unit 0
        // If in shader has layout(binding = 0), this don't required
        //ourShader.setInt("lumaTexture", 0);
        CHECK_GL_ERROR_ABORT();

        /* ------------ ChromaTexture aka UV ------------- */
        // Bind our texture in Texture Unit 1 ChromaTexture
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, uvSample);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bufferDim.x >> 1, bufferDim.y, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                     frame._dataPointer);
        CHECK_GL_ERROR_ABORT();
        // Set our "chromaTexture" sampler to use Texture Unit 1
        // If in shader has layout(binding = 0), this don't required
        //ourShader.setInt("chromaTexture", 1);
        CHECK_GL_ERROR_ABORT();

        //
        if (bPixelDescChnaged)
        {
            // pvd->_vrender->setTexture(pvd->_vtexture);
            // pvd->_vrender->setTextureRect(ax::Rect{Vec2::ZERO, Vec2{
            //                                                        frame._videoDim.x / AX_CONTENT_SCALE_FACTOR(),
            //                                                        frame._videoDim.y / AX_CONTENT_SCALE_FACTOR(),
            //                                                    }});

            if (pixelFormat >= MEVideoPixelFormat::YUY2)
            {
                // auto ps = pvd->_vrender->getProgramState();
                // PrivateVideoDescriptor::updateColorTransform(ps, frame._vpd._fullRange);
                // ourShader.setMat4("colorTransform", colorTransform);

                //unsigned int lights_index = glGetUniformBlockIndex(shaderA.ID, "Lights");
                //glUniformBlockBinding(shaderA.ID, lights_index, 2);
                // instead: layout(std140, binding = 0) uniform UBO
                glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo);
                
                CHECK_GL_ERROR_ABORT();
            }

            // pvd->_scaleDirty = true;
        }
    });

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        framebuffer_size_callback(window, 1024, 768);

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // bind Texture
        // glBindTexture(GL_TEXTURE_2D, texture);

        if (!mEngine->renderVideoFrame())
        { // render default
            ourShader.use();
        }
        // ... render video sample by OpenGL, DirectX, vulkan, Metal

        // render container
        //ourShader.use();

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteBuffers(1, &ubo);
    glDeleteTextures(1, &ySample);
    glDeleteTextures(1, &uvSample);

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
    glViewport(-121, 0, 1267, height);
}
