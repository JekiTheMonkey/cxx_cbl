#include "Compiler/Command.hpp"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

static char buf[jcbl::Command::MaxLength] = { 0 };

namespace jcbl
{

int Command::getString(size_t argNumber, const char **str) const
{
    const bool valid = argNumber < m_argc_;
    *str = valid ? m_args_[argNumber] : 0;
    return valid ? 0 : -1;
}

int Command::getInt(size_t argNumber, int *num) const
{
    const bool valid = argNumber < m_argc_;
    *num = valid ? strtol(m_args_[argNumber], 0, 10) : 0;
    return valid ? 0 : -1;
}

void Command::printArgs() const
{
    for (size_t i = 0; i <= m_argc_; i++)
        printf("%ld. \"%s\"\n", i + 1, m_args_[i]);
}

const char *Command::getCommandLine() const
{
    size_t len = 0;
    for (size_t i = 0; i < m_argc_; i++)
        len = sprintf(buf + len, "%s ", m_args_[i]);
    return buf;
}

#define CMP_HDL(cmd) do { if (strcasecmp(str, #cmd) == 0) return cmd; } while(0)
Command::ID Command::determineType(const char *str)
{
    CMP_HDL(Print);
    CMP_HDL(PrintLoop);
    CMP_HDL(Newline);
    return Unknown;
}
#undef CMP_HDL

#define CMP_HDL(cmd) do { if (strncasecmp(buf, #cmd, n) == 0) return cmd; } while(0)
Command::ID Command::determineType(const char *buf, size_t n)
{
    CMP_HDL(Print);
    CMP_HDL(PrintLoop);
    CMP_HDL(Newline);
    return Unknown;
}
#undef CMP_HDL

} // namespace jcbl
