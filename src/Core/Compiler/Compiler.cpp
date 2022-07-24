#include "Compiler/Compiler.hpp"
#include "Utility/ArgumentsParser.hpp"

#include <cassert>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#ifndef COMP_BUF_SIZE
#define COMP_BUF_SIZE 16384
#endif

#ifndef ERR_BUF_SIZE
#define ERR_BUF_SIZE 4096
#endif

#ifndef FILE_FLAG
#define FILE_FLAG "-f"
#endif

#ifndef OUT_FLAG
#define OUT_FLAG "-o"
#endif

#ifndef PREPROCESS_FLAG
#define PREPROCESS_FLAG "-E"
#endif

#ifndef OUT_EXT
#define OUT_EXT "out"
#endif

#ifndef PREPROCESS_OUT_EXT
#define PREPROCESS_OUT_EXT "p"
#endif

void close_fd(int &fd);
const char *skip_spaces(const char *buf, size_t len);

namespace jcbl
{

static char err_buf[ERR_BUF_SIZE];

int Compiler::process(const char **args)
{
    // Initialize parser to retrieve arguments for compilation.
    m_argParser_.acquireArguments(args, true);

    // Open given file.
    m_file_.in_fd = openInputFile();
    if (m_file_.in_fd == Failed)
    {
        releaseData();
        return Failed;
    }

    // Open file to write to.
    computeOutputFilename();
    m_file_.out_fd = openOutputFile();
    if (m_file_.out_fd == Failed)
    {
        releaseData();
        return Failed;
    }

    // Compile the input file, write the result into output file.
    const int code = run();
    if (code == Failed)
    {
        releaseData();
        return Failed;
    }

    // Erase data for a next compilation.
    releaseData();
    return Success;
}

int Compiler::run()
{
    int code, bytes, fd = m_file_.in_fd;
    size_t rem = 0, tokenlen;
    char stop_on = '\n', buf[COMP_BUF_SIZE];
    const char *begin, *end;
    bool comment;

    // Start each binary file with a control header.
    code = createHeader();
    if (code == Failed)
    {
        m_errno_ = R_FailedToWrite;
        return Failed;
    }

    // Main parsing loop.
    while ((bytes = read(fd, buf + rem, sizeof(buf) - rem)) > 0)
    {
        m_tokenParser_.giveBuffer(buf, bytes);
        while (m_tokenParser_.getRemainingBytes() > 0)
        {
            code = m_tokenParser_.retrieveToken(&begin, &end, stop_on);
            if (code == TokenParser::Failed && m_tokenParser_.isLineOver())
            {
                m_errno_ = R_MissingQuote;
                return Failed;
            }
            // Skip comment tokens.
            if (comment && code != TokenParser::Finished)
                continue;
            // New portion of data has to be appended.
            if (code == TokenParser::BufferEnded)
                break;
            // Don't stop on lf next time, but carry on switching lines.
            stop_on = code == TokenParser::Finished ? -1 : '\n';
            tokenlen = end - begin;

            // Skip comments or null tokens. (null tokens should be fixed tho)
            comment = *begin == Command::CommentSign;
            if (tokenlen == 0 || comment)
                continue;

            // Parse string parameter into binary format.
            if (!comment)
                code = appendToBinary(begin, end, m_tokenParser_.isLineOver(),
                    m_tokenParser_.getLastTokenType());
        }

        // Get how many unanalyzed bytes have remained.
        rem = m_tokenParser_.getRemainingBytes();
    }

    if ((code == TokenParser::BufferEnded && rem > 0) || bytes == -1)
        return Failed;
    return Success;
}

int Compiler::createHeader()
{
    assert(m_file_.out_fd);
    const int code = write(m_file_.out_fd, CTRL_HEADER, sizeof(CTRL_HEADER));
    return code != -1 ? Success : Failed;
}

int Compiler::appendToBinary(
    const char *begin, const char *end, bool lineIsOver, TokenParser::Type type)
{
    assert(begin);
    assert(end);
    assert(end - begin <= Command::MaxLength);

    int code, fd = m_file_.out_fd;
    const size_t len = end - begin;
    char buf[Command::MaxLength + sizeof(int)] = { 0 };
    size_t bytes = 0;

    // Store data to write into a local buffer.
    const Command::ID cmdId = Command::determineType(begin, len);
    if (cmdId != Command::Unknown)
    {
        memcpy(buf, &cmdId, sizeof(int));
        bytes = sizeof(int);
    }
    else
    {
        if (type == TokenParser::Number)
        {
            const int num = strtol(begin, 0, 10);
            memcpy(buf, &num, sizeof(num));
            bytes = sizeof(num);
        }
        else if (type == TokenParser::String)
        {
            memcpy(buf, begin, len);
            bytes = len;
        }
    }

    // Write binary data into output file.
    code = write(fd, buf, bytes + lineIsOver * sizeof(int));
    if (code == -1)
    {
        m_errno_ = R_FailedToWrite;
        return Failed;
    }
    return Success;
}

void Compiler::printError(const char *str) const
{
    // Error is stored in err_buf.
    const char *err = getError();

    // Avoid reformatting if string is null or empty.
    if (str && *str != '\0')
    {
        // err_buf "Some error." -> "Passed message: Some error.".
        const char post_msg[] = ": ";
        const size_t errlen = strlen(err);
        const size_t msglen = strlen(str);
        const size_t postlen = sizeof(post_msg) - 1;
        memmove(err_buf + msglen + postlen, err_buf, errlen + 1); // +1 due '\0'
        memcpy(err_buf + msglen, post_msg, postlen);
        memcpy(err_buf, str, msglen);
    }
    fputs(err_buf, stderr);
}

// Store the returned error string in a global buffer relatively to this unit.
#define WRITE_ERROR(...) snprintf(err_buf, ERR_BUF_SIZE, __VA_ARGS__)
#define CASE(r_code, ...) case r_code: WRITE_ERROR(__VA_ARGS__); return err_buf
const char *Compiler::getError() const
{
    switch(m_errno_)
    {
        CASE(R_Success,         "Success");
        CASE(R_NoInputFile,     "No input file.");
        CASE(R_FailedOpenFile,  "Failed opening file - %s.", strerror(errno));
        CASE(R_NoOutputName,    "No output filename.");
        CASE(R_MissingQuote,    "Missing quote.");
        CASE(R_LineTooLong,     "Line too long.");
        CASE(R_CommentTooLong,  "Comment too long.");
        CASE(R_FailedToWrite,   "Failed to write to output file -%s.",
            strerror(errno));
        default:
            WRITE_ERROR("Unknown error code.");
            return err_buf;
    }

}
#undef WRITE_ERROR

int Compiler::openInputFile()
{
    const char *filename = m_argParser_.getArgument(FILE_FLAG);
    if (!filename)
    {
        m_errno_ = R_NoInputFile;
        return Failed;
    }
    m_file_.in_filename = filename;

    const int fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        m_errno_ = R_FailedOpenFile;
        return Failed;
    }
    return fd;
}

int Compiler::openOutputFile()
{
    const char *filename = m_file_.out_filename;
    assert(filename);

    const int fd = open(filename, O_WRONLY|O_CREAT|O_TRUNC, 0666);
    if (fd == -1)
    {
        m_errno_ = R_FailedOpenFile;
        return Failed;
    }
    return fd;
}

void Compiler::computeOutputFilename()
{
    // Use static memory to save output filename.
    enum { MaxFilenameLen = 256 };
    static char buf[MaxFilenameLen];
    m_file_.out_filename = buf;

    // Use given output filename if specified.
    const char *out = m_argParser_.getArgument(OUT_FLAG);
    if (out)
    {
        strcpy(buf, out);
        return;
    }

    const char *in = m_file_.in_filename;
    assert(in);

    // Find out if there is an extension.
    const char *dot = static_cast<const char*>(rawmemchr(in, '.'));

    // Copy just filename to buffer.
    if (dot)
        memcpy(buf, in, dot - in);

    // Append .OUT_EXT to the filename including the trailing zero.
    memcpy(buf + (dot ? dot - in : 0), "." OUT_EXT, sizeof(OUT_EXT) + 1);
    printf("%s\n", buf);
}

void Compiler::releaseData()
{
    m_argParser_.releaseArguments();
    m_file_.in_filename = 0;
    m_file_.out_filename = 0;

    // Close and invalidate FDs.
    close_fd(m_file_.in_fd);
    close_fd(m_file_.out_fd);
}

} // namespace jcbl

void close_fd(int &fd)
{
    if (fd != jcbl::Compiler::InvalidFd)
    {
        close(fd);
        fd = jcbl::Compiler::InvalidFd;
    }
}

const char *skip_spaces(const char *buf, size_t len)
{
    for (size_t i = 0; i < len; i++)
        if (!isspace(*buf))
            return buf + i;
    return buf + len;
}
