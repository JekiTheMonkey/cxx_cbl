#include "Command.hpp"
#include "Script.hpp"
#include "ScriptInterpeter.hpp"
#include "Utility.hpp"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#define strndup(buf, n) cxx_strndup(buf, n)

int count_lines(int fd, size_t &lines);
int fill_commands(cbl::Command **commands, int fd);

namespace cbl
{

ScriptInterpreter::~ScriptInterpreter()
{
    deleteMatrix(m_scripts_);
}

// Errno is restored after close() in order to keep the one got from a
// function just before it
#define CLOSE(fd) do { int tmperr = errno; close(fd); errno = tmperr; } while(0)
bool ScriptInterpreter::loadScript(const char *filepath)
{
    int fd = open(filepath, O_RDONLY);
    if (fd == -1)
        return false;

    size_t lines = 0;
    int code = count_lines(fd, lines);
    printf("Command amount: %lu\n", lines);

    CLOSE(fd);
    if (code == -1)
        return false;

    fd = open(filepath, O_RDONLY);
    if (fd == -1)
        return false;
    Command **commands = new Command*[lines];
    code = fill_commands(commands, fd);

    CLOSE(fd);
    if (code == -1)
    {
        deleteMatrix(commands);
        return false;
    }

    char *filepathCopy = strdup(filepath);
    insertScript(new Script(commands, filepathCopy));
    return true;
}

bool ScriptInterpreter::unloadScript(const char *filepath)
{
    size_t i = 0;
    for (; i < m_size_; i++)
        if (strcmp(m_scripts_[i]->getFilepath(), filepath) == 0)
            break;
    if (i == m_size_)
        return false;
    return unloadScript(i);
}

bool ScriptInterpreter::unloadScript(size_t index)
{
    if (index >= m_size_)
        return false;

    // maybe halt it somehow beforehand
    delete m_scripts_[index];
    m_scripts_[index] = 0;
    return true;
}

void ScriptInterpreter::unloadScripts()
{
    deleteMatrix(m_scripts_);
}

void ScriptInterpreter::run()
{
    while (m_size_)
    {
        for (size_t i = 0; i < m_size_; i++)
        {
            if (m_scripts_[i])
            {
                if (!m_scripts_[i]->run() && !unloadScript(i))
                {
                    fprintf(stderr,
                        "The requsted script to unload has not been found\n");
                }
            }
        }
    }
}

Script **ScriptInterpreter::findEmptyScript()
{
    for (size_t i = 0; i < m_size_; i++)
        if (!m_scripts_[i])
            return m_scripts_ + i;
    return 0;
}

size_t ScriptInterpreter::insertScript(Script *script)
{
    Script **cell = findEmptyScript();
    size_t index;
    if (cell)
    {
        *cell = script;
        index = cell - m_scripts_;
    }
    else
    {
        const size_t oldSize = m_size_;
        reallocScriptsArray(m_size_ == 0 ? 8 : m_size_ * 2);
        m_scripts_[oldSize] = script;
        index = oldSize;
    }
    return index;
}

void ScriptInterpreter::reallocScriptsArray(size_t newSize)
{
    assert(m_size_ < newSize);

    Script **newScriptsArray = new Script*[newSize];

    for (size_t i = 0; i < newSize; i++)
        newScriptsArray[i] = i < m_size_ ? m_scripts_[i] : 0;
    if (m_scripts_)
        delete[] m_scripts_;
    m_scripts_ = newScriptsArray;
    m_size_ = newSize;
}

} // namespace cbl

int count_lines(int fd, size_t &lines)
{
    int bytes;
    bool isComment = false;
    char buf[16384];
    while ((bytes = read(fd, buf, sizeof(buf))) > 0)
    {
        for (int i = 0; i < bytes; i++)
        {
            if (buf[i] == '\n' || buf[i] == '\0')
            {
                if (!isComment)
                    lines++;
                isComment = false;
            }
            if (buf[i] == '\n' && i + 1 < bytes &&
                buf[i + 1] == cbl::Command::CommentSign)
                isComment = true;
        }
    }
    return bytes == -1 ? -1 : 0;
}

int count_spaces(const char *buf, int len)
{
    int c = 0;
    for (; len; len--, buf++)
        if (*buf == ' ' || *buf == '\n' || *buf == '\0')
            c++;
    return c;
}

int retrieve_arguments(const char *buf, size_t len, char **args, size_t &argc)
{
    bool specialCharacter = false;
    char tokenLimiter = ' ';
    const char *begin = buf, *end = 0;
    for (size_t i = 0; i < len; i++)
    {
        if (buf[i] == tokenLimiter)
        {
            end = buf + i;
            args[argc] = strndup(begin, end - begin);
            begin = end + 1 + (tokenLimiter == '"'); // Skip the quote
            argc++;
        }
        if (buf[i] == '"')
        {
            if (!specialCharacter)
            {
                if (tokenLimiter != '"')
                    begin++; // Skip the quote
                tokenLimiter = tokenLimiter != '"' ? '"' : ' ';
            }
            else
                specialCharacter = false;
        }
        if (buf[i] == '\\')
            specialCharacter = true;
    }
    if (tokenLimiter == '"')
    {
        // The message shouldn't appear on fail,
        // but be accessible to the client instead
        fprintf(stderr, "Mismatching quotes.\n");
        return -1;
    }
    if (!end || end - begin > 0)
    {
        args[argc] = strndup(begin, end ? end - begin : buf + len - buf);
        argc++;
    }
    return 0;
}

cbl::Command *analyze_text(const char *buf, size_t len)
{
    size_t argc = 0;
    char *localArgs[cbl::Command::MaxArguments] = { 0 };
    if (retrieve_arguments(buf, len, localArgs, argc) == -1)
    {
        for (size_t i = 0; i < argc; i++)
            delete localArgs[i];
        return 0;
    }

    assert(argc > 0);
    char **args =
        static_cast<char**>(memdup(localArgs, (argc + 1) * sizeof(void*)));
    const int type = cbl::Command::determineType(*args);
    return new cbl::Command(type, args);
}

#define PRINT_LINE_TOO_LONG(line) \
    fprintf(stderr, "Line number %ld is too long.\n", line)
int fill_commands(cbl::Command **commands, int fd)
{
    int bytes, used = 0, linelen;
    size_t line = 0;
    char buf[16384];
    const char *begin, *end;
    while ((bytes = read(fd, buf + used, sizeof(buf) - used)) > 0)
    {
        printf("Read bytes: %d\n", bytes);
        used += bytes;
        begin = buf;
        while (begin)
        {
            end = static_cast<const char*>(memchr(begin, '\n', bytes));
            if (!end)
            {
                if (used >= cbl::Command::MaxLength)
                {
                    used = 0;
                    PRINT_LINE_TOO_LONG(line);
                }
                break;
            }
            linelen = end - begin;
            used -= linelen;
            printf("Command length: %d\n", linelen);
            if (linelen && linelen <= cbl::Command::MaxLength &&
                *begin != cbl::Command::CommentSign)
            {
                cbl::Command *cmd = analyze_text(begin, linelen);
                if (cmd)
                {
                    commands[line] = cmd;
                    commands[line]->printArgs();
                    line++;
                    printf("New command has been analyzed\n");
                }
                else
                {
                    fprintf(stderr, "A null command has been returned\n");
                }
            }
            else if (linelen > cbl::Command::MaxLength)
                PRINT_LINE_TOO_LONG(line);
            begin = used > 0 ? end + 1 : 0;
        }
    }
    commands[line] = 0; // terminating null
    return bytes == -1 ? -1 : 0;
}

