#ifndef SCRIPT_INTERPRETER_HPP
#define SCRIPT_INTERPRETER_HPP

#include <cstddef>

/*
*   Possible optimization:
*       - Use a hash-tree instead of linked list of 'Item's to increase
*           unloadScript() lookup for given script filename.
*       - Create an array which will store active scripts in first cells instead
*           of being sparsed as they're now; the current array still has to
*           exist along side in order to make findEmptyScript() be O(1) instead
*           of O(n), where n is size of m_scripts_ array.
*/

namespace cbl
{

class Script;

class ScriptInterpreter
{
public:
    ScriptInterpreter()
        : m_scripts_(0), m_size_(0) { }
    ~ScriptInterpreter();

    bool loadScript(const char *filepath);
    bool unloadScript(const char *filepath);
    bool unloadScript(size_t index);
    void unloadScripts();
    void run();

private:
    ScriptInterpreter(ScriptInterpreter&);
    ScriptInterpreter &operator=(ScriptInterpreter&);

    Script **findEmptyScript();
    /// @brief Insert given script and returns its index inside the container.
    /// @param script Script to insert.
    size_t insertScript(Script *script);
    void reallocScriptsArray(size_t newSize);

private:
    Script **m_scripts_;
    size_t m_size_;
};

} // namespace cbl

#endif // SCRIPT_INTERPRETER_HPP
