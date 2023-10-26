#ifndef UNDEFBHVR_LEXER_HPP
#define UNDEFBHVR_LEXER_HPP

#include <cctype>
#include <istream>
#include <iterator>
#include <string>


namespace lexer
{

struct token
{
    enum tok_type
    {
        ID,
        REG,
        NUM,
        SYM,
        STR,
        CHAR,
        SEC,
        END
    } type;
    std::string content;
    unsigned line = 0;
};

struct token_stream
{
    struct iterator
    {
        iterator &operator++()
        {
            str->next();
            return *this;
        }
        const token &operator*() const
        {
            return str->current;
        }
        token *operator->()
        {
            return &str->current;
        }

        iterator()
            : str(nullptr)
        {
        }

        iterator &operator=(const iterator &other)
        {
            str = other.str;
            return *this;
        }

        bool operator==(const std::default_sentinel_t &)
        {
            return (*this)->type == token::END;
        }

    private:
        iterator(token_stream *str)
            : str(str)
        {
        }
        token_stream *str = nullptr;
        friend struct token_stream;
    };

    token_stream(std::istream &is)
        : is(&is)
    {
        next();
    }

    iterator iter()
    {
        return iterator(this);
    }

    iterator begin()
    {
        return iter();
    }

    std::default_sentinel_t end()
    {
        return std::default_sentinel;
    }

private:
    std::istream *is;
    token current;
    char c = ' ';
    void next()
    {
        while (isspace(c) && c != EOF)
        {
            if (c == '\n') ++current.line;
            c = is->get();
        }
        if (c == '#')
        {
            while (c != '\n' && c != EOF) c = is->get();
            c = is->get();
        }
        current.content.clear();
        if (c == '$')
        {
            current.type = token::REG;
            c = is->get();
            while (isalnum(c) || c == '_') current.content.push_back(c), c = is->get();
        }
        else if (c == '_' || isalpha(c))
        {
            current.type = token::ID;
            while (isalnum(c) || c == '_') current.content.push_back(c), c = is->get();
        }
        else if (c == '.')
        {
            current.type = token::SEC;
            current.content.push_back(c);
            c = is->get();
            while (c != EOF && !isspace(c)) current.content.push_back(c), c = is->get();
        }
        else if (isdigit(c) || c == '+' || c == '-')
        {
            current.type = token::NUM;
            current.content.push_back(c);
            if (c == '0')
            {
                c = is->get();
                if (c == 'x')
                {
                    current.content.push_back(c);
                    c = is->get();
                    while (isdigit(c) || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
                        current.content.push_back(c), c = is->get();
                }
                else
                {
                    while (isdigit(c) && c < '8') current.content.push_back(c), c = is->get();
                }
            }
            else
            {
                c = is->get();
                while (isdigit(c)) current.content.push_back(c), c = is->get();
            }
        }
        else if (c == '"')
        {
            current.type = token::STR;
            current.content.push_back(c);
            c = is->get();
            while (c != '"')
            {
                current.content.push_back(c);
                if (c == '\\')
                {
                    c = is->get();
                    current.content.push_back(c);
                }
                c = is->get();
            }
            current.content.push_back(c);
            c = is->get();
        }
        else if (c == EOF)
        {
            current.type = token::END;
        }
        else
        {
            current.type = token::SYM;
            current.content.push_back(c);
            c = is->get();
        }
    }
};

inline int parse_int(const std::string &num)
{
    if (num == "0") return 0;
    int res = 0;
    if (num.starts_with("0x"))
    {
        for (std::size_t i = 2; i != num.size(); ++i)
        {
            res *= 16;
            if (isdigit(num[i]))
                res += num[i] - '0';
            else if (isupper(num[i]))
                res += num[i] - 'A' + 10;
            else if (islower(num[i]))
                res += num[i] - 'a' + 10;
        }
        return res;
    }
    if (num[0] == 0)
    {
        for (std::size_t i = 1; i != num.size(); ++i)
        {
            res *= 8;
            res += num[i] - '0';
        }
        return res;
    }
    bool sym = 0;
    if (num[0] == '-') sym = 1;
    for (std::size_t i = isdigit(num[0]) ? 0 : 1; i != num.size(); ++i)
    {
        res *= 10;
        res += num[i] - '0';
    }
    return sym ? -res : res;
}

} // namespace lexer

#endif
