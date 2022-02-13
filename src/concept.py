class RenderBuffer:
    renderbuffer : int
    width : int
    height : int

    def resize(width, height): pass


class SharedRenderBuffer(RenderBuffer):
    map<context, frambuffer> framebuffers;

    def init(contexts):
        for context in contexts:
            add(context)

    def add(context):
        buffer = createFramebuffer()
        bind(buffer, renderbuffer)
        framebuffers[context] = buffer

    def use(context):
        glfwMakeContextCurrent(context);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffers[context]);
        glViewport(width, height);

    def copyToBuffer(context, buffer=0):
        glfwMakeContextCurrent(context);
        bind(read, framebuffers[context])
        bind(draw, buffer)
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);


class Controller:
    host : Window
    presentation : Window
    renderbuffer : SharedRenderBuffer

    def init():
        host()
        presentation()
        renderbuffer.add(host, Write)
        renderbuffer.add(presentation, Read)

    def Render():
        host.use(clear=True)
        scene.Render(host.camera)
        host.swap()

        if (presentation):
            renderbuffer.use(host)
            scene.Render(presentation.camera)
            renderbuffer.copyToBuffer(presentation)
