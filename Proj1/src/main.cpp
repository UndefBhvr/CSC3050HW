#include <instr_parse/MIPS.hpp>
#include <las/labelTable.h>
#include <las/phase1.h>
#include <las/phase2.h>
#include <lex/lexer.hpp>

#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <ostream>
#include <string>
#include <vector>

// using std::cout;
using std::cerr;
using std::filesystem::exists;

int main(int argc, char **argv)
{
    if(argc<3||argc>5)
    {
        cerr<<"Invalid arguments! Expect two arguments.\n"
        <<"Usage: las [path_to_input] [path_to_output] [[expected]]\n";
        return -1;
    }
    if(!exists(argv[1]))
    {
        cerr<<"Input file \""<<argv[1]<<"\" not found!\n";
        return -1;
    }
    std::ifstream fasm(argv[1]);
    lexer::token_stream stream(fasm);
    auto it = stream.iter();
    while (it->type != lexer::token::SEC || it->content != ".text")
    {
        ++it;
    }
    ++it;
    unsigned pc = 0x100000;
    while (it->type != lexer::token::END && it->content != ".data")
    {
        parse_instr(it, pc);
        ++pc;
    }
    fasm.close();
    fasm.open(argv[1]);
    std::ofstream fout(argv[2]);
    lexer::token_stream stream2(fasm);
    it = stream2.iter();
    while (it->type != lexer::token::SEC || it->content != ".text") ++it;
    ++it;
    pc = 0x100000;
    while (it->type != lexer::token::END && it->content != ".data")
    {
        parse_instr2(it, fout, pc);
        ++pc;
    }
}
