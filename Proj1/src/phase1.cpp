#include <las/phase1.h>
#include <instr_parse/MIPS.hpp>
#include <las/labelTable.h>
#include <tokenize/lexer.hpp>

#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <ostream>
#include <string>
#include <vector>

// using std::cout;
using std::cerr;

MIPS::instr parse_instr(lexer::token_stream::iterator &it, unsigned pc)
{
    if (it->type != lexer::token::ID)
    {
        cerr << "Program at line " << it->line << ": an iden expected, but \"" << it->content
             << "\" found.\n";
        std::terminate();
    }
    std::string name = it->content;
    ++it;
    while (it->content == ":")
    {
        if (label_table[name])
        {
            cerr << "Label redefined: " << label_table[name] << " have occured before.\n";
            std::terminate();
        }
        label_table[name] = pc;
        ++it;
        if (it->type != lexer::token::ID)
        {
            cerr << "Program at line " << it->line << ": an iden expected, but \"" << it->content
                 << "\" found.\n";
            std::terminate();
        }
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
    auto __ = instr.parser->parse(it);
    return instr;
}
