#include "ScriptInterpeter.hpp"

#include <cstdio>
#include <cstring>
#include <errno.h>

void load_scripts(jcbl::ScriptInterpreter &interpeter, int argc, char **argv)
{
    for (int i = 1; i < argc; i++)
    {
        if (interpeter.loadScript(argv[i]))
            fprintf(stderr,
                "Script \"%s\" has been successfully loaded.\n", argv[i]);
        else
            fprintf(stderr, "Failed to load \"%s\": %s",
                argv[i], strerror(errno));
    }
}

int main(int argc, char **argv)
{
    jcbl::ScriptInterpreter interpeter;
    load_scripts(interpeter, argc, argv);
    interpeter.run();
    return 0;
}
