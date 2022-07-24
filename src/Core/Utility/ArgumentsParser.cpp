#include "Utility/ArgumentsParser.hpp"
#include "Utility/Utility.hpp"

#include <cassert>
#include <cstring>

ArgumentsParser::ArgumentsParser()
: m_sharing_(false)
, m_argc_(0)
, m_argv_(0)
{

}

ArgumentsParser::ArgumentsParser(const char **argv, bool share)
: m_sharing_(share)
, m_argc_(0)
, m_argv_(0)
{
    if (argv)
        acquireArguments(argv, share);
}

ArgumentsParser::~ArgumentsParser()
{
    freeArguments();
}

const char *ArgumentsParser::getArgument(const char *option) const
{
    size_t index;
    const int code = getOptionIndex(option, index);
    return code == 0 ? m_argv_[index + 1] : 0;
}

bool ArgumentsParser::optionExists(const char *option) const
{
    size_t index;
    return getOptionIndex(option, index) == 0;
}

void ArgumentsParser::acquireArguments(const char **argv, bool share)
{
    assert(argv);
    m_sharing_ = share;

    // Count argc
    for (; argv[m_argc_]; m_argc_++)
        {   }

    if (share)
    {
        // Remove const because arguments will not be altered
        m_argv_ = const_cast<char**>(argv);
    }
    else if (m_argc_ > 0)
    {
        // Duplicate argv
        m_argv_ = new char*[m_argc_];
        for (size_t i = 0; *argv; i++, argv++)
            m_argv_[i] = cxx_strndup(*argv, strlen(*argv));
    }
}

void ArgumentsParser::releaseArguments()
{
    freeArguments();
    m_sharing_ = false;
    m_argv_ = 0;
    m_argc_ = 0;
}

int ArgumentsParser::getOptionIndex(const char *option, size_t &index) const
{
    for (index = 0; index < m_argc_; index++)
        if (strcmp(m_argv_[index], option) == 0)
            return 0;
    return -1;
}

void ArgumentsParser::freeArguments()
{
    if (!m_sharing_)
        delete_str_array(m_argv_);
}
