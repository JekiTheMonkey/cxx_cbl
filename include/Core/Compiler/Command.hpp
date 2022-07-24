#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "Utility/NonCopyable.hpp"
#include "Utility/Utility.hpp"

namespace jcbl
{

class Command : private NonCopyable
{
public:
    // All the commands identifiers should be available in
    // Command::determineType().
    enum ID
    {
        Print,
        PrintLoop,
        Newline,
        Total,
        Unknown = -1
    };

    enum { MaxLength = 4096, CommentSign = '#', MaxArguments = 256 };

public:
    /// @brief Construct a Command object that will own the passed strings.
    /// @param type Command type.
    /// @param args Null terminating array of null terminating strings.
    Command(int type, char **args)
        : m_type_(type), m_argc_(0), m_args_(args)
        { for (; *args; args++, m_argc_++); }
    /// @brief Free passed array of strings.
    ~Command()
        { delete_str_array(m_args_); }

    int getType() const { return m_type_; }
    int getString(size_t argNumber, const char **str) const;
    int getInt(size_t argNumber, int *num) const;

    void printArgs() const;
    const char *getCommandLine() const;
    static ID determineType(const char *str);
    static ID determineType(const char *buf, size_t n);

private:
    int m_type_;
    size_t m_argc_;
    char **m_args_;
};

} // namespace jcbl

#endif // COMMAND_HPP
