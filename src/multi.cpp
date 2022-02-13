//////////////////////////////////////////////////////////////////////////////
//
//  Triangles.cpp
//
//////////////////////////////////////////////////////////////////////////////
#include <cstdio>
#include "vgl.h"
#include "LoadShaders.h"

enum VAO_IDs { Triangles, NumVAOs };
enum Buffer_IDs { ArrayBuffer, NumBuffers };
enum Attrib_IDs { vPosition = 0 };

GLuint  VAOs[NumVAOs];
GLuint  Buffers[NumBuffers];

const GLuint  NumVertices = 6;
////////////////////////////////////
//RBO variables
enum {Color=0, NumRenderBuffers=1, NumFBOs=4};
GLuint renderbuffer[NumRenderBuffers], fbos[NumFBOs];
GLuint buffwidth = 3200;
GLuint buffheight = 600;



//----------------------------------------------------------------------------
//
// init
//

void
init( void )
{
    glCreateRenderbuffers(NumRenderBuffers, renderbuffer);
    glNamedRenderbufferStorage(renderbuffer[Color], GL_RGBA, buffwidth, buffheight);
    glGenFramebuffers(1, &fbos[0]);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbos[0]);
    glNamedFramebufferRenderbuffer(fbos[0], GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderbuffer[Color]);
    printf("\"init()\", is RB valid: %i\n", glIsRenderbuffer(renderbuffer[Color]));
    printf("\"init()\", is FBO valid: %i\n", glCheckNamedFramebufferStatus(fbos[0], GL_DRAW_FRAMEBUFFER));

    glGenVertexArrays( NumVAOs, VAOs );
    glBindVertexArray( VAOs[Triangles] );

    GLfloat  vertices[NumVertices][2] = {
        { -0.90f, -0.90f }, { -0.90f,  0.90f }, {  0.00f, -0.90f },  // Triangle 1
        {  0.00f,  0.90f }, {  0.90f,  0.90f }, {  0.90f, -0.90f }   // Triangle 2
    };

    glCreateBuffers( NumBuffers, Buffers );
    glBindBuffer( GL_ARRAY_BUFFER, Buffers[ArrayBuffer] );
    glBufferStorage( GL_ARRAY_BUFFER, sizeof(vertices), vertices, 0);

    ShaderInfo  shaders[] =
    {
        { GL_VERTEX_SHADER, "media/shaders/triangles/triangles.vert" },
        { GL_FRAGMENT_SHADER, "media/shaders/triangles/triangles.frag" },
        { GL_NONE, NULL }
    };

    GLuint program = LoadShaders( shaders );
    glUseProgram( program );

    glVertexAttribPointer( vPosition, 2, GL_FLOAT,
                           GL_FALSE, 0, BUFFER_OFFSET(0) );
    glEnableVertexAttribArray( vPosition );
}

void
init2( const int idx_in )
{
    glGenFramebuffers(1, &fbos[idx_in]);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbos[idx_in]);
    glEnable(GL_RENDERBUFFER);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer[Color]);

    glNamedFramebufferRenderbuffer(fbos[idx_in], GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderbuffer[Color]);
}

//----------------------------------------------------------------------------
//
// display
//

void
display( void )
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbos[0]);
    glViewport(0, 0, buffwidth, buffheight);
    static const float black[] = { 0.2f, 0.2f, 0.2f, 0.0f };
    static const float redish[] = { 0.6f, 0.4f, 0.3f, 0.0f };
    glClearBufferfv(GL_COLOR, 0, black);

    glBindVertexArray( VAOs[Triangles] );
    glDrawArrays( GL_TRIANGLES, 0, NumVertices );


    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbos[0]);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glViewport(0, 0, 800, 600);
    glClearBufferfv(GL_COLOR, 0, redish);

    glBlitFramebuffer(0, 0, 800, 600, 0, 0, 800, 600, GL_COLOR_BUFFER_BIT, GL_NEAREST);

}

void
display2( const int idx_in )
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbos[idx_in]);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glViewport(0, 0, 800, 600);

    glBlitFramebuffer(800 * idx_in - 1, 0, 800 * idx_in - 1 + 800, 600, 0, 0, 800, 600, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}


//----------------------------------------------------------------------------
//
// main
//

#ifdef _WIN32
int CALLBACK WinMain(
  _In_ HINSTANCE hInstance,
  _In_ HINSTANCE hPrevInstance,
  _In_ LPSTR     lpCmdLine,
  _In_ int       nCmdShow
)
#else
int
main( int argc, char** argv )
#endif
{
    // Initialize GLFW
    glfwInit();

    //TODO Create Windows Class
    // Create Window Hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    // Create Windows
    GLFWwindow* window = glfwCreateWindow(800, 600, "Triangles", NULL, NULL);
    GLFWwindow* window2 = glfwCreateWindow(800, 600, "Triangles2", NULL, window);
    GLFWwindow* window3 = glfwCreateWindow(800, 600, "Triangles3", NULL, window);
    GLFWwindow* window4 = glfwCreateWindow(800, 600, "Triangles4", NULL, window);

    // Set window positions
    glfwSetWindowPos(window,  100, 100);
    glfwSetWindowPos(window2, 900, 100);
    glfwSetWindowPos(window3, 1700, 100);
    glfwSetWindowPos(window4, 2500, 100);

    // Initialize Windows
    glfwMakeContextCurrent(window);
    // Initialize gl3w - thanks @Ripi2 for the assist
    gl3wInit();
    init();
    glfwMakeContextCurrent(window2);
    init2(1);
    glfwMakeContextCurrent(window3);
    init2(2);
    glfwMakeContextCurrent(window4);
    init2(3);


    // Draw the Windows
    while (!glfwWindowShouldClose(window) && !glfwWindowShouldClose(window2) && !glfwWindowShouldClose(window3) && !glfwWindowShouldClose(window4))
    {
        glfwMakeContextCurrent(window);
        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
        glFinish();

        glfwMakeContextCurrent(window2);
        display2(1);
        glfwSwapBuffers(window2);
        glfwPollEvents();
        glFinish();

        glfwMakeContextCurrent(window3);
        display2(2);
        glfwSwapBuffers(window3);
        glfwPollEvents();

        glfwMakeContextCurrent(window4);
        display2(3);
        glfwSwapBuffers(window4);
        glfwPollEvents();
    }


    // Destroy Windows
    glfwDestroyWindow(window);
    glfwDestroyWindow(window2);
    glfwDestroyWindow(window3);
    glfwDestroyWindow(window4);


    // Terminate GLFW Instance
    glfwTerminate();
}