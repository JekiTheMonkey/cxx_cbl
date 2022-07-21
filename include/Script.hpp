#ifndef SCRIPT_HPP
#define SCRIPT_HPP

#include <cstddef>

namespace jcbl
{

class Command;

/// @brief Class that contains and manages commands.
class Script
{

public:
    /// @brief Construct a Script object that will own the passed commands.
    /// @param commands Null terminating array of commands.
    Script(Command **commands, char *filepath)
        : m_commands_(commands), m_currentCommand_(0), m_filepath_(filepath) { }
    /// @brief Free passed array of commands.
    ~Script();

    /// @brief Execute commands until the end or a break command.
    /// @details The run() executes commands until the end or a break command by
    /// calling their respective handlers. Once commands are finished the
    /// function will return false, true otherwise.
    bool run();

    const char *getFilepath() const { return m_filepath_; }

private:
    Script(Script&);
    Script &operator=(Script&);

    int handlePrintCommand(const Command &cmd);
    int handlePrintLoopCommand(const Command &cmd);
    int handleNewlineCommand(const Command &cmd);

    void printError(const Command &cmd) const;

private:
    Command **m_commands_;
    size_t m_currentCommand_;
    char *m_filepath_;
};

} // namespace jcbl

#endif // SCRIPT_HPP
