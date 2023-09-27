#ifndef UNDEFBHVR_LEXER_HPP
#define UNDEFBHVR_LEXER_HPP

#include <cctype>
#include <istream>
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
    unsigned line=0;
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

private:
    std::istream *is;
    token current;
    char c = ' ';
    void next()
    {
        while (isspace(c) && c != EOF)
        {
            if(c=='\n')++current.line;
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
            while (c!=':')
            {
                if(!isspace(c))current.content.push_back(c), c = is->get();
            }
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
        else if (c=='"')
        {
            current.type = token::STR;
            current.content.push_back(c);
            c=is->get();
            while(c!='"')
            {
                current.content.push_back(c);
                if(c=='\\')
                {
                    c=is->get();
                    current.content.push_back(c);
                }
                c=is->get();
            }
            current.content.push_back(c);
            c=is->get();
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

} // namespace lexer

#endif