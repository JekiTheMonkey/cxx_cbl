#include "ScriptInterpeter.hpp"

#include <cstdio>
#include <cstring>
#include <errno.h>

typedef bool(cbl::ScriptInterpreter::*memfoo)(const char*);
void memory_manager(cbl::ScriptInterpreter &interpreter, memfoo callback,
    int argc, char **argv, const char *operation)
{
    for (int i = 1; i < argc; i++)
    {
        if ((interpreter.*callback)(argv[i]))
        {
            printf("Script \"%s\" has been successfully %sed.\n", argv[i],
                operation);
        }
        else
        {
            fprintf(stderr, "Failed to %sed \"%s\": %s",
                operation, argv[i], strerror(errno));
        }
    }
}

int main(int argc, char **argv)
{
    using namespace cbl;

    ScriptInterpreter interpeter;

    memory_manager(
        interpeter, &ScriptInterpreter::loadScript, argc, argv, "load");
    /* interpeter.run(); */
    memory_manager(
        interpeter, &ScriptInterpreter::unloadScript, argc, argv, "unload");
    return 0;
}
