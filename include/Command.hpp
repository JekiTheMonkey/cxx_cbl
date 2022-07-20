#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "Utility.hpp"

namespace cbl
{

class Command
{
public:
    enum
    {
        Print,
        PrintLoop,
        Newline,
        Break,
        Unknown = -1
    };

    enum { MaxLength = 4096, CommentSign = '#', MaxArguments = 256 };

public:
    Command(int type, char **args)
        : m_type_(type), m_args_(args) { }
    ~Command() { deleteMatrix(m_args_); }
    int getType() const { return m_type_; }
    void printArgs() const;
    static int determineType(const char *cmd);

private:
    Command(Command&);
    Command &operator=(Command&);

private:
    int m_type_;
    char **m_args_;
};

} // namespace cbl

#endif // COMMAND_HPP
