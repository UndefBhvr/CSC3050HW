#ifndef UNDEFBHVR_PHASE1_H
#define UNDEFBHVR_PHASE1_H

#include <instr_parse/MIPS.hpp>
#include <lex/lexer.hpp>

MIPS::instr parse_instr(lexer::token_stream::iterator &it, unsigned pc);

#endif