#include <lexer.hpp>
#include <loader.h>
#include <memory.hpp>
#include <register.hpp>

#include <array>
#include <cctype>
#include <concepts>
#include <iostream>

namespace xldz
{
/*
constexpr char escape_table[128]{
    ['0'] = 0,
    ['\\'] = '\\',
    ['n'] = '\n',
    ['t'] = '\t',
    ['a'] = '\a',
    ['b'] = '\b',
    ['r'] = '\r',
    ['"'] = '"',
    ['\''] = '\'',
};
*/

constexpr std::array<char, 128> escape_table = []()
{
    std::array<char, 128> res{0};
    res['0'] = 0;
    res['\\'] = '\\';
    res['n'] = '\n';
    res['t'] = '\t';
    res['a'] = '\a';
    res['b'] = '\b';
    res['r'] = '\r';
    res['"'] = '"';
    res['\''] = '\'';
    return res;
}();

std::string escape_string(const std::string &str)
{
    std::string res;
    res.reserve(str.size());
    bool escaping = 0;
    for (char c : str)
    {
        if (escaping)
            res.push_back(escape_table[c]), escaping = 0;
        else if (c == '\\')
            escaping = 1;
        else
            res.push_back(c);
    }
    return res;
}

u64 align_4(u64 s)
{
    return (s / 4 + !!(s % 4)) * 4;
}

void load_data(std::istream &is)
{
    lexer::token_stream str(is);
    auto it = str.begin();
    while (it->type != lexer::token::SEC || it->content != ".data") ++it;
    ++it;
    while (it->type != lexer::token::END &&
           (it->type != lexer::token::SEC || it->content != ".text"))
    {
        if (it->type == lexer::token::SYM || it->type == lexer::token::ID)
        {
            if (it->type == lexer::token::ID) XLDZ_DEBUGF("Loading data %s\n", it->content.c_str());
            ++it;
            continue;
        }
        if (it->type == lexer::token::SEC)
        {
            std::string data_type = it->content;
            ++it;
            if (data_type == ".ascii")
            {
                if (it->type != lexer::token::STR)
                {
                    std::terminate();
                }
                std::string escaped = escape_string(it->content);
                u64 s = align_4(escaped.size() - 2);
                for (u64 i = 1; i < escaped.size() - 1; ++i)
                {
                    data_pos[i - 1] = escaped[i];
                }
                data_pos += s;
                ++it;
                continue;
            }
            if (data_type == ".asciiz")
            {
                if (it->type != lexer::token::STR)
                {
                    std::terminate();
                }
                std::string escaped = escape_string(it->content);
                u64 s = align_4(escaped.size() - 1);
                for (u64 i = 1; i < escaped.size() - 1; ++i)
                {
                    data_pos[i - 1] = escaped[i];
                }
                data_pos[escaped.size() - 1] = 0;
                data_pos += s;
                ++it;
                continue;
            }
            if (data_type == ".byte" || data_type == ".half" || data_type == ".word")
            {
                char c = data_type[1];
                u32 mask = c == 'b' ? 0xFFU : (c == 'h' ? 0xFFFFU : 0xFFFFFFFFU);
                u32 len = c == 'b' ? 1 : (c == 'h' ? 2 : 4);
                if (it->type != lexer::token::NUM)
                {
                    data_pos += 4;
                    continue;
                }
                u32 cnt = 0;
                do {
                    u32 res = lexer::parse_int(it->content);
                    ++it;
                    res &= mask;
                    for (u32 i = 0; i < 4; ++i)
                        data_pos[i + cnt] = (res & (0xFFU << (8 * i))) >> (8 * i);
                    cnt += len;
                } while (it->content == "," && (++it, 1));
                data_pos += align_4(cnt);
                continue;
            }
            std::terminate();
        }
    }
}

void load_instr(binary_u32_stream &instr_stream)
{
    u32 addr = TEXT_BEGIN;
    for (u32 instr : instr_stream)
    {
        sw_to(addr, instr);
        XLDZ_DEBUGF("%08x at addr %08x\n", w_at(addr), addr);
        addr += 4;
    }
}

} // namespace xldz
