#include "Command.hpp"
#include "Script.hpp"
#include "Utility.hpp"

#include <cstdio>

int handle_print(const char *str);
int handle_print_loop(const char *str, size_t n);
int handle_newline();

namespace jcbl
{

Script::~Script()
{
    delete_matrix(m_commands_);
    delete[] m_filepath_;
}

bool Script::run()
{
    int code;
    bool run = true, res = false;
    while (run)
    {
        const Command *cmd = m_commands_[m_currentCommand_];
        if (!cmd)
            run = false;
        else
        {
            // Maybe create a container containing member functions and store
            // then accordingly to the commands identificators and loop through
            // them instead.
            switch (cmd->getType())
            {
            case Command::Print:
                code = handlePrintCommand(*cmd);
                break;
            case Command::PrintLoop:
                code = handlePrintLoopCommand(*cmd);
                break;
            case Command::Newline:
                code = handleNewlineCommand(*cmd);
                break;
            case Command::Break:
                run = false;
                break;
            default:
                fprintf(stderr, "Unknown command. Aborting script...\n");
                break;
            }
            res = code != -1;
            run = code != -1;
            m_currentCommand_++;
        }
    }
    return res;
}

int Script::handlePrintCommand(const Command &cmd)
{
    const char *str;
    if (cmd.getString(1, &str) == -1)
    {
        printError(cmd);
        return -1;
    }
    handle_print(str);
    return 0;
}

int Script::handlePrintLoopCommand(const Command &cmd)
{
    const char *str;
    int num;
    if (cmd.getString(1, &str) == -1 ||
        cmd.getInt(2, &num) == -1)
    {
        printf("About to print an error\n");
        printError(cmd);
        return -1;
    }
    handle_print_loop(str, num);
    return 0;
}

int Script::handleNewlineCommand(const Command&)
{
    handle_newline();
    return 0;
}

void Script::printError(const Command &cmd) const
{
    fprintf(stderr, "An error occured while extracting arguments from line "
        "\"%s\"\n", cmd.getCommandLine());
}

} // namespace jcbl

int handle_print(const char *str)
{
    printf("%s", str);
    return 0;
}

int handle_print_loop(const char *str, size_t n)
{
    for (size_t i = 0; i < n; i++)
        printf("%s\n", str);
    return 0;
}

int handle_newline()
{
    putchar(10);
    return 0;
}
