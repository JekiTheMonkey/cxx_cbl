#include "Command.hpp"

#include <cstdio>
#include <cstring>

namespace cbl
{

void Command::printArgs() const
{
    size_t i = 0;
    for (; m_args_[i]; i++)
        printf("%ld. \"%s\"\n", i + 1, m_args_[i]);
    printf("%ld. \"%s\"\n", i + 1, m_args_[i]);
}

#define CMP_HDL(cmd) do { if (strcasecmp(str, #cmd) == 0) return cmd; } while(0)
int Command::determineType(const char *str)
{
    CMP_HDL(Print);
    CMP_HDL(PrintLoop);
    CMP_HDL(Newline);
    CMP_HDL(Break);
    return Unknown;
}

}
