#ifndef COMPILER_HPP
#define COMPILER_HPP

#include "Compiler/Command.hpp"
#include "Utility/ArgumentsParser.hpp"
#include "Utility/TokenParser.hpp"
#include "Utility/NonCopyable.hpp"

#ifndef CTRL_HEADER
#define CTRL_HEADER "cxx_jcbl prototype version 24/06/2022"
#endif

namespace jcbl
{

class Compiler : private NonCopyable
{
public:
    enum
    {
        Success = 0,
        Failed = -1,
        InvalidFd = -1
    };

private:
    enum Action
    {
        Preprocess,
        All
    };

    enum Result
    {
        R_Success,
        R_NoInputFile,
        R_FailedOpenFile,
        R_NoOutputName,
        R_MissingQuote,
        R_LineTooLong,
        R_CommentTooLong,
        R_FailedToWrite,
    };

public:
    Compiler()
        : m_argParser_(), m_tokenParser_(), m_file_(), m_errno_(R_Success)
        { }
    int process(const char **args);
    void printError(const char *str) const;
    const char *getError() const;

private:
    int run();
    int createHeader();
    int appendToBinary(const char *begin, const char *end, bool lineIsOver,
        TokenParser::Type type);

    int openInputFile();
    int openOutputFile();
    void computeOutputFilename();
    void releaseData();

private:
    struct ActiveFile
    {
        ActiveFile()
            : in_filename(0), out_filename(0),
            in_fd(InvalidFd), out_fd(InvalidFd)
            { }
        const char *in_filename;
        const char *out_filename;
        int in_fd;
        int out_fd;
    };

private:
    ArgumentsParser m_argParser_;
    TokenParser m_tokenParser_;
    ActiveFile m_file_;
    Result m_errno_;
};

} // namespace jcbl

#endif // COMPILER_HPP
