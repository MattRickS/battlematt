#include <glad/glad.h>

#include <controller/Application.h>


int main(int numArgs, char* args[])
{
    stbi_set_flip_vertically_on_load(true);

    Application app = Application();
    if (!app.IsInitialised())
        return 1;

    if (numArgs > 1)
        app.controller->Load(args[1]);

    app.Exec();

    return 0;
}
