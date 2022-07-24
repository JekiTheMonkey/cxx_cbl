#ifndef TOKEN_PARSER_HPP
#define TOKEN_PARSER_HPP

#include "Utility/NonCopyable.hpp"

#include <cstddef>

class TokenParser : private NonCopyable
{
public:
    enum
    {
        Success = 0,
        Finished,
        BufferEnded,
        Failed = -1,
    };

    enum Type { Number, String, Unknown = -1 };

public:
    TokenParser();
    TokenParser(const char *buffer, size_t length);
    void giveBuffer(const char *buffer, size_t length);
    int retrieveToken(
        const char **begin, const char **end, char delim = -1);
    size_t getRemainingBytes() const { return m_length_ - m_analyzed_; }
    bool isLineOver() const { return m_buffer_[m_index_] == '\n'; }
    Type getLastTokenType() const { return m_lastTokenType_; }

private:
    void skipSpaces(char except_ch = -1);

private:
    const char *m_buffer_;
    size_t m_length_;
    size_t m_index_;
    size_t m_analyzed_;
    Type m_lastTokenType_;
};

#endif // TOKEN_PARSER_HPP
