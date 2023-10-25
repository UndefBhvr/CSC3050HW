#ifndef XLDZ_ALU_HPP
#define XLDZ_ALU_HPP

#include <prelude.hpp>
#include <register.hpp>

namespace xldz
{

enum alu_op : u8
{
    ADD,
    SUB,
    AND,
    NAND,
    OR,
    NOR,
    XOR,
    SL,
    SRL,
    SRA,
    SLT,
    SGEQ,
    SLTI,
    SGEQI,
    SEQ,
    SNE,
};

inline void alu(alu_op fn, u32 sval, u32 tval, u8 dreg)
{
    switch (fn)
    {
    case ADD: reg[dreg] = sval + tval; break;
    case SUB: reg[dreg] = sval - tval; break;
    case AND: reg[dreg] = sval & tval; break;
    case NAND: reg[dreg] = ~(sval & tval); break;
    case OR: reg[dreg] = sval | tval; break;
    case NOR: reg[dreg] = ~(sval | tval); break;
    case XOR: reg[dreg] = sval ^ tval; break;
    case SL: reg[dreg] = sval << tval; break;
    case SRL: reg[dreg] = sval >> tval; break;
    case SRA: reg[dreg] = static_cast<i32>(sval) >> tval; break;
    case SLT: reg[dreg] = sval < tval; break;
    case SGEQ: reg[dreg] = sval >= tval; break;
    case SLTI: reg[dreg] = static_cast<i32>(sval) < static_cast<i32>(tval); break;
    case SGEQI: reg[dreg] = static_cast<i32>(sval) >= static_cast<i32>(tval); break;
    case SEQ: reg[dreg] = (sval == tval); break;
    case SNE: reg[dreg] = (sval != tval); break;
    }
}

} // namespace xldz

#endif
