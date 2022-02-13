void Init()
{
    // host context
    host = createWindow()
    offScreenFrameBuffer = createFramebuffer()
    renderbuffer = createRenderBuffer()
    bindRenderToFrame(offScreenFrameBuffer, renderbuffer)

    // presentation context
    presentation = createWindow(share=host)
    tempFramebuffer = createFramebuffer()
    bindRenderToFrame(tempFramebuffer, renderbuffer)
}

void Render()
{
    // host context
    glfwMakeContextCurrent(host.window);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);  // default framebuffer
    glViewport(host.width, host.height);
    glClear(GL_COLOR_BUFFER_BIT);
    renderScene(hostCamera);
    glfwSwapBuffers(host.window);

    // Resizing presentation window will need to resize the renderbuffer
    // https://stackoverflow.com/questions/44763449/updating-width-and-height-of-render-target-on-the-fly
    if (presentation.isActive)
    {
        // off screen rendering within the host context
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, offScreenFrameBuffer);
        glViewport(presentation.dimensions);
        glClear(GL_COLOR_BUFFER_BIT);
        renderScene(presentationCamera);

        // presentation context
        glfwMakeContextCurrent(presentation);
        copyRenderToFrame(tempFramebuffer, defaultFramebuffer)  // glBlit
    }
}

// ---------------------------------
// Off screen render setup

// Host context
GLuint renderbuffer;
glCreateRenderbuffers(1, &renderbuffer);
glNamedRenderbufferStorage(renderbuffer, GL_RGBA, width, height);
GLuint offScreenFrameBuffer;
glGenFramebuffers(1, &offScreenFrameBuffer);
glBindFramebuffer(GL_DRAW_FRAMEBUFFER, offScreenFrameBuffer);
glNamedFramebufferRenderbuffer(offScreenFrameBuffer, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderbuffer);

// Presentation context
GLuint tempFramebuffer;
glGenFramebuffers(1, &tempFramebuffer);
glBindFramebuffer(GL_READ_FRAMEBUFFER, tempFramebuffer);
glEnable(GL_RENDERBUFFER);
glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
glNamedFramebufferRenderbuffer(tempFramebuffer, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderbuffer);

// ---------------------------------
// Copy render to screen - can possibly just use glBlitNamedFramebuffer

glBindFramebuffer(GL_READ_FRAMEBUFFER, tempFramebuffer);
glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
glViewport(0, 0, width, height);
//            src(x0, y0, x1, y1)  dst(x0, y0, x1, y1)
glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
