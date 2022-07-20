#ifndef SCRIPT_HPP
#define SCRIPT_HPP

#include <cstddef>

namespace cbl
{

class Command;

/// @brief Class that contains and manages commands.
class Script
{

public:
    /// @brief Construct a Script object that will own the passed commands.
    /// @param commands Null terminated commands array.
    Script(Command **commands, char *filepath)
        : m_commands_(commands), m_currentCommand_(0), m_filepath_(filepath) { }
    /// @brief Release commands array.
    ~Script();

    /// @brief Execute commands until the end or a break command.
    /// @details The run() executes commands until the end or a break commandby
    /// calling their respective handlers. Once commands are finished the
    /// function will return false, true otherwise.
    bool run();

    const char *getFilepath() const { return m_filepath_; }

private:
    Script(Script&);
    Script &operator=(Script&);

private:

private:
    Command **m_commands_;
    size_t m_currentCommand_;
    char *m_filepath_;
};

} // namespace cbl

#endif // SCRIPT_HPP
