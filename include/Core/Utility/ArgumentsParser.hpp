#ifndef ARGUMENTS_PARSER_HPP
#define ARGUMENTS_PARSER_HPP

#include "Utility/NonCopyable.hpp"

#include <cstddef>

class ArgumentsParser : private NonCopyable
{
public:
    ArgumentsParser();
    ArgumentsParser(const char **argv, bool share = false);
    ~ArgumentsParser();

    const char *getArgument(const char *option) const;
    bool optionExists(const char *option) const;

    void acquireArguments(const char **argv, bool share = false);
    void releaseArguments();
    bool containsArguments() const { return m_argv_; }

private:
    int getOptionIndex(const char *option, size_t &index) const;
    void freeArguments();

private:
    bool m_sharing_;
    size_t m_argc_;
    char **m_argv_;
};

#endif // ARGUMENTS_PARSER_HPP
