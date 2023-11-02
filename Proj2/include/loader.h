#ifndef XLDZ_LOADER_H
#define XLDZ_LOADER_H

#include <prelude.hpp>

#include <concepts>
#include <iostream>

namespace xldz
{

struct binary_u32_stream
{
    std::istream *is;
    binary_u32_stream(std::istream &is)
        : is(&is)
    {
        next();
    }

    u32 current;
    bool eof = false;

    bool strip()
    {
        char c = is->peek();
        while (c != '0' && c != '1')
        {
            is->get();
            c = is->peek();
            if (c == EOF) return false;
        }
        return true;
    }

    bool next()
    {
        if (!strip())
        {
            eof = true;
            return false;
        }
        current = 0;
        char c = is->peek();
        while (c == '0' || c == '1')
        {
            current <<= 1;
            current |= (c - '0');
            is->get();
            c = is->peek();
        }
        return true;
    }

    struct iterator
    {
        binary_u32_stream *str;

        iterator &operator++()
        {
            str->next();
            return *this;
        }

        iterator operator++(int)
        {
            str->next();
            return *this;
        }

        u32 operator*() const
        {
            return str->current;
        }

        bool operator==(std::default_sentinel_t)
        {
            return str->eof;
        }
    };

    iterator begin()
    {
        return iterator(this);
    }

    std::default_sentinel_t end()
    {
        return std::default_sentinel;
    }
};



void load_data(std::istream &is);
void load_instr(binary_u32_stream &instr_stream);

} // namespace xldz

#endif
