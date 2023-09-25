#include <las/phase2.h>
#include <instr_parse/MIPS.hpp>
#include <las/labelTable.h>
#include <lex/lexer.hpp>


#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <ostream>
#include <string>
#include <vector>

// using std::cout;
using std::cerr;

unsigned codegen(const MIPS::instr &instr, const MIPS::Data_field &data, unsigned pc)
{
    unsigned res = instr.op << 26U;
    switch (instr.type)
    {
    case MIPS::R:
        res |= data.rs << 21 | data.rt << 16 | data.rd << 11 | data.sa << 6 | instr.fn;
        break;
    case MIPS::I:
        if (data.lab.empty())
            res |= data.imm & 0xffff;
        else
            res |= (label_table[data.lab] - pc - 1) & 0xff;
        res |= data.rs << 21 | data.rt << 16;
        break;
    case MIPS::J: res |= label_table[data.lab] & 0x3ffffff; break;
    }
    return res;
}

MIPS::instr parse_instr2(lexer::token_stream::iterator &it, std::ostream &os, unsigned pc)
{
    std::string name = it->content;
    ++it;
    while (it->content == ":")
    {
        ++it;
        name = it->content;
        ++it;
    }
    auto i = MIPS::mapper.find(name);
    if (i == MIPS::mapper.end())
    {
        cerr << "Program at line " << it->line << ": invalid instruction \"" << name
             << "\" found.\n";
        std::terminate();
    }
    auto [_, instr] = *i;
    auto data = instr.parser->parse(it);
    auto code = codegen(instr, data, pc);
    for (unsigned i = 1 << 31; i; i >>= 1) os << (int)!!(code & i);
    os << '\n';
    return instr;
}
