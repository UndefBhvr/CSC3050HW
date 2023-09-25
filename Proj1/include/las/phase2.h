#ifndef UNDEFBHVR_PHASE2_H
#define UNDEFBHVR_PHASE2_H

#include <instr_parse/MIPS.hpp>
#include <lex/lexer.hpp>

unsigned codegen(const MIPS::instr &instr, const MIPS::Data_field &data, unsigned pc);
MIPS::instr parse_instr2(lexer::token_stream::iterator &it, std::ostream &os, unsigned pc);

#endif