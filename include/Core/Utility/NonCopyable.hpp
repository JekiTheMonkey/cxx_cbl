#ifndef NON_COPYABLE_HPP
#define NON_COPYABLE_HPP

/// @brief Class that disallows copy.
class NonCopyable
{
protected:
    NonCopyable() {}
    ~NonCopyable() {}

private:
    NonCopyable(const NonCopyable&);
    NonCopyable &operator=(const NonCopyable&);
};

#endif // NON_COPYABLE_HPP
