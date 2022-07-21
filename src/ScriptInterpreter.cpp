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

// Use C++ version instead of C ones.
#define strndup(buf, n) cxx_strndup(buf, n)

int count_lines(int fd, size_t &lines);
int fill_commands(jcbl::Command **commands, int fd);

namespace jcbl
{

ScriptInterpreter::~ScriptInterpreter()
{
    delete_matrix(m_scripts_);
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

    CLOSE(fd);
    if (code == -1)
        return false;

    fd = open(filepath, O_RDONLY);
    if (fd == -1)
        return false;
    Command **commands = new Command*[lines + 1]; // +1 for null terminating ptr
    code = fill_commands(commands, fd);

    CLOSE(fd);
    if (code == -1)
    {
        delete_matrix(commands);
        return false;
    }

    char *filepathCopy = strdup(filepath);
    insertScript(new Script(commands, filepathCopy));
    m_loaded_++;
    return true;
}

bool ScriptInterpreter::unloadScript(const char *filepath)
{
    size_t i = 0;
    for (; i < m_size_; i++)
    {
        if (m_scripts_[i] &&
            strcmp(m_scripts_[i]->getFilepath(), filepath) == 0)
            break;
    }
    if (i == m_size_)
        return false;
    return unloadScript(i);
}

bool ScriptInterpreter::unloadScript(size_t index)
{
    if (index >= m_size_)
        return false;

    // Maybe halt it somehow beforehand
    assert(m_scripts_[index]);
    delete m_scripts_[index];
    m_scripts_[index] = 0;
    m_loaded_--;
    return true;
}

void ScriptInterpreter::unloadScripts()
{
    delete_matrix(m_scripts_);
}

void ScriptInterpreter::run()
{
    while (m_loaded_)
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
        // Reallocate the scripts container in case of unsufficient capacity.
        const size_t oldSize = m_size_;
        reallocScriptsArray(m_size_ == 0 ? 8 : m_size_ * 2);

        m_scripts_[oldSize] = script;
        index = oldSize;
    }
    return index;
}

void ScriptInterpreter::reallocScriptsArray(size_t newSize)
{
    Script **newContainer = new Script*[newSize];

    // Copy old scripts into new array, fill empty cells with zeroes.
    for (size_t i = 0; i < newSize; i++)
        newContainer[i] = i < m_size_ ? m_scripts_[i] : 0;

    // Release scripts that don't fit into new array.
    for (size_t i = newSize; i < m_size_; i++)
        unloadScript(i);

    if (m_scripts_)
        delete[] m_scripts_;
    m_scripts_ = newContainer;
    m_size_ = newSize;
}

} // namespace jcbl

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
                buf[i + 1] == jcbl::Command::CommentSign)
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
    char tokenLimiter = ' ';
    const char *begin = buf, *end = 0;
    for (size_t i = 0; i < len; i++)
    {
        if (buf[i] == tokenLimiter)
        {
            end = buf + i;
            args[argc] = strndup(begin, end - begin);
            begin = end + 1; // Skip the quote.
            argc++;
        }
        if (buf[i] == '"')
        {
            if (tokenLimiter != '"')
                begin++; // Skip the quote.
            if (tokenLimiter == '"')
                i++; // Skip next space.
            tokenLimiter = tokenLimiter != '"' ? '"' : ' ';
        }
    }
    if (tokenLimiter == '"')
    {
        // The message shouldn't appear on fail,
        // but be accessible to the client instead
        fprintf(stderr, "Mismatching quotes.\n");
        return -1;
    }
    if (begin > end)
    {
        args[argc] = strndup(begin, buf + len - buf);
        argc++;
    }
    return 0;
}

jcbl::Command *analyze_text(const char *buf, size_t len)
{
    size_t argc = 0;
    char *localArgs[jcbl::Command::MaxArguments] = { 0 };
    if (retrieve_arguments(buf, len, localArgs, argc) == -1)
    {
        for (size_t i = 0; i < argc; i++)
            delete[] localArgs[i];
        return 0;
    }

    assert(argc > 0);
    char **args = new char*[argc + 1];
    memcpy(args, localArgs, (argc + 1) * sizeof(void *));
    const int type = jcbl::Command::determineType(*args);
    return new jcbl::Command(type, args);
}

#define PRINT_LINE_TOO_LONG(line) \
    fprintf(stderr, "Line number %ld is too long.\n", line)
int fill_commands(jcbl::Command **commands, int fd)
{
    int bytes, used = 0, linelen;
    size_t line = 0;
    char buf[16384];
    const char *begin, *end;
    while ((bytes = read(fd, buf + used, sizeof(buf) - used)) > 0)
    {
        used += bytes;
        begin = buf;
        while (begin)
        {
            end = static_cast<const char*>(memchr(begin, '\n', bytes));
            if (!end)
            {
                if (used >= jcbl::Command::MaxLength)
                {
                    used = 0;
                    PRINT_LINE_TOO_LONG(line);
                }
                break;
            }
            linelen = end - begin;
            used -= linelen;
            if (linelen && linelen <= jcbl::Command::MaxLength &&
                *begin != jcbl::Command::CommentSign)
            {
                jcbl::Command *cmd = analyze_text(begin, linelen);
                if (cmd)
                {
                    commands[line] = cmd;
                    line++;
                }
            }
            else if (linelen > jcbl::Command::MaxLength)
                PRINT_LINE_TOO_LONG(line);
            begin = used > 0 ? end + 1 : 0;
        }
    }
    commands[line] = 0; // terminating null
    return bytes == -1 ? -1 : 0;
}

