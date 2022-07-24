#include "Utility/TokenParser.hpp"

#include <cctype>
#include <cstdio>
#include <cstdlib>

TokenParser::TokenParser()
: m_buffer_(0)
, m_length_(0)
, m_index_(0)
, m_analyzed_(0)
, m_lastTokenType_(Unknown)
{

}

TokenParser::TokenParser(const char *buffer, size_t length)
: m_buffer_(buffer)
, m_length_(length)
, m_index_(0)
, m_analyzed_(0)
, m_lastTokenType_(Unknown)
{

}

void TokenParser::giveBuffer(const char *buffer, size_t length)
{
    m_buffer_ = buffer;
    m_length_ = length;
    m_index_ = 0;
    m_analyzed_ = 0;
}

int TokenParser::retrieveToken(
    const char **begin, const char **end, char delim)
{
    *begin = *end = 0;
    skipSpaces(delim);
    *begin = m_buffer_ + m_analyzed_;

    const bool quote = **begin == '"';
    if (quote)
    {
        // Start from character after the first quote.
        (*begin)++;
        m_index_++;
    }
    m_lastTokenType_ = quote ? String : Number;

    // Search for a required character and return if found.
    for (; m_index_ < m_length_; m_index_++)
    {
        // Found asked character.
        if (m_buffer_[m_index_] == delim)
        {
            if (quote)
                return Failed;
            *end = m_buffer_ + m_index_;
            skipSpaces(delim);
            return *end > *begin ? Success : Finished;
        }

        // Found second quote.
        if (quote && m_buffer_[m_index_] == '"')
        {
            *end = m_buffer_ + m_index_;
            m_index_++; // Skip the quote.
            skipSpaces(delim);
            return m_buffer_[m_analyzed_ + 1] != delim ? Success : Finished;
        }

        // Found a regular delimeter.
        if (!quote && isspace(m_buffer_[m_index_]))
        {
            m_analyzed_ = m_index_;
            *end = m_buffer_ + m_index_;
            skipSpaces(delim);
            return m_buffer_[m_analyzed_ + 1] != delim ? Success : Finished;
        }
    }

    // End of token has not been found.
    return BufferEnded;
}

void TokenParser::skipSpaces(char except_ch)
{
    for (;
        m_index_ < m_length_ &&
        m_buffer_[m_index_] != except_ch && isspace(m_buffer_[m_index_]);
        m_index_++)
    {
    }
    m_analyzed_ = m_index_;
}
