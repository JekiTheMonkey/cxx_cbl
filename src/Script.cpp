#include "Command.hpp"
#include "Script.hpp"
#include "Utility.hpp"

namespace cbl
{

Script::~Script()
{
    deleteMatrix(m_commands_);
    delete[] m_filepath_;
}

bool Script::run()
{
    return false;
}

} // namespace cbl
