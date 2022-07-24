#include "Core/Compiler/Compiler.hpp"
#include "Core/VM/VM.hpp"

#include <cstdio>
#include <cstring>
#include <errno.h>

void load_scripts(jcbl::VM &vm, int argc, char **argv)
{
    for (int i = 1; i < argc; i++)
    {
        if (vm.loadScript(argv[i]))
            fprintf(stderr,
                "Script \"%s\" has been successfully loaded.\n", argv[i]);
        else
            fprintf(stderr, "Failed to load \"%s\": %s",
                argv[i], strerror(errno));
    }
}

int main(int argc, char **argv)
{
    jcbl::Compiler compiler;
    const int code = compiler.process(const_cast<const char**>(argv + 1));
    if (code == -1)
        compiler.printError("Compilation error");
    return 0;

    // Unused until compilator will not be finished.
    (void)(argc);
    /* jcbl::VM vm; */
    /* load_scripts(vm, argc, argv); */
    /* vm.run(); */
    /* return 0; */
}
