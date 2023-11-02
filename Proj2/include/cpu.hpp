#ifndef XLDZ_CPU_HPP
#define XLDZ_CPU_HPP

#include <alu.hpp>
#include <cstdlib>
#include <memory.hpp>
#include <prelude.hpp>
#include <register.hpp>
#include <syscall.hpp>

#include <bit>

namespace xldz
{

enum class op_code : u8
{
    spec1 = 0x00,
    bltz_bgez = 0x01,
    j = 0x02,
    jal = 0x03,
    beq = 0x04,
    bne = 0x05,
    blez = 0x06,
    bgtz = 0x07,
    addi = 0x08,
    addiu = 0x09,
    slti = 0x0A,
    sltiu = 0x0B,
    andi = 0x0C,
    ori = 0x0D,
    xori = 0x0E,
    lui = 0x0F,
    _control_cop0 = 0x10,
    _fp_cop1 = 0x11,
    _cop2 = 0x12,
    _beql = 0x14,
    _bnql = 0x15,
    _blezl = 0x16,
    _bgtzl = 0x17,
    spec2 = 0x1C,
    lb = 0x20,
    lh = 0x21,
    lwl = 0x22,
    lw = 0x23,
    lbu = 0x24,
    lhu = 0x25,
    lwr = 0x26,
    sb = 0x28,
    sh = 0x29,
    swl = 0x2A,
    sw = 0x2B,
    swr = 0x2E,
    _cache = 0x2F,
    _ll = 0x30,
    _lwc1 = 0x31,
    _lwc2 = 0x32,
    _pref = 0x33,
    _ldc1 = 0x35,
    _ldc2 = 0x36,
    _sc = 0x38,
    _swc1 = 0x39,
    _swc2 = 0x3A,
    _sdc1 = 0x3D,
    _sdc2 = 0x3E,
};

enum class fn_code : u8
{
    sll = 0x00,
    _movf = 0x01,
    srl = 0x02,
    sra = 0x03,
    sllv = 0x04,
    srlv = 0x06,
    srav = 0x07,
    jr = 0x08,
    jalr = 0x09,
    movz = 0x0A,
    movn = 0x0B,
    syscall = 0x0C,
    _break_ = 0x0D,
    _sync = 0x0F,
    mfhi = 0x10,
    mthi = 0x11,
    mflo = 0x12,
    mtlo = 0x13,
    mult = 0x18,
    multu = 0x19,
    div = 0x1A,
    divu = 0x1B,
    add = 0x20,
    addu = 0x21,
    sub = 0x22,
    subu = 0x23,
    and_ = 0x24,
    or_ = 0x25,
    xor_ = 0x26,
    nor = 0x27,
    slt = 0x2A,
    sltu = 0x2B,
};

enum class sp2_code : u8
{
    madd = 0x00,
    maddu = 0x01,
    mul = 0x02,
    msub = 0x04,
    msubu = 0x05,
    clz = 0x20,
    clo = 0x21,
    _sdbbp = 0x3f,
};

using exec_handle = void (*)(u32);

inline void trap(u32)
{
    exit(-1);
}

inline void execute(u32);
inline void execute_delayslot(u32 addr);

inline const exec_handle sp1[64] = {
    [](u32 i) // sll 0x00
    { reg[RD(i)] = reg[RT(i)] << SA(i); },
    trap,     // _movf 0x01
    [](u32 i) // srl 0x02
    { reg[RD(i)] = reg[RT(i)] >> SA(i); },
    [](u32 i) // sra 0x03
    { reg[RD(i)] = static_cast<i32>(reg[RT(i)] >> SA(i)); },
    [](u32 i) // sllv 0x04
    { reg[RD(i)] = reg[RT(i)] << reg[RS(i)]; },
    trap,     // 0x05
    [](u32 i) // srlv 0x06
    { reg[RD(i)] = reg[RT(i)] >> reg[RS(i)]; },
    [](u32 i) // srav 0x07
    { reg[RD(i)] = static_cast<i32>(reg[RT(i)] >> reg[RS(i)]); },
    [](u32 i) // jr 0x08
    {
        u32 _pc = pc + 4;
        pc = reg[RS(i)] - 4;
        // execute_delayslot(_pc);
    },
    [](u32 i) // jalr 0x09
    {
        u32 _pc = pc + 4;
        reg[RD(i)] = pc + 4; // 8;
        pc = reg[RS(i)] - 4;
        // execute_delayslot(_pc);
    },
    [](u32 i) // movz 0x0A
    {
        if (!reg[RT(i)]) reg[RD(i)] = reg[RS(i)];
    },
    [](u32 i) // movn 0x0B
    {
        if (reg[RT(i)]) reg[RD(i)] = reg[RS(i)];
    },
    // TODO
    [](u32 i) // syscall 0x0C
    { syscall(); },
    trap,     // _break_ 0x0D
    trap,     // 0x0E
    trap,     // _sync 0x0F
    [](u32 i) // mfhi 0x10
    { reg[RD(i)] = hi; },
    [](u32 i) // mthi 0x11
    { hi = reg[RS(i)]; },
    [](u32 i) // mflo 0x12
    { reg[RD(i)] = lo; },
    [](u32 i) // mtlo 0x13
    { lo = reg[RS(i)]; },
    trap,     // 0x14
    trap,     // 0x15
    trap,     // 0x16
    trap,     // 0x17
    [](u32 i) // mult 0x18
    {
        u64 res = 1LL * static_cast<i32>(reg[RS(i)]) * static_cast<i32>(reg[RT(i)]);
        hi = res >> 32ULL;
        lo = res & 0xFFFFFFFFULL;
    },
    [](u32 i) // mult 0x19
    {
        u64 res = 1ULL * reg[RS(i)] * reg[RT(i)];
        hi = res >> 32ULL;
        lo = res & 0xFFFFFFFFULL;
    },
    [](u32 i) // div 0x1A
    {
        lo = static_cast<i32>(reg[RS(i)]) / static_cast<i32>(reg[RT(i)]);
        hi = static_cast<i32>(reg[RS(i)]) % static_cast<i32>(reg[RT(i)]);
    },
    [](u32 i) // div 0x1B
    {
        lo = reg[RS(i)] / reg[RT(i)];
        hi = reg[RS(i)] % reg[RT(i)];
    },
    trap,     // 0x1C
    trap,     // 0x1D
    trap,     // 0x1E
    trap,     // 0x1F
    [](u32 i) // add 0x20
    { reg[RD(i)] = reg[RS(i)] + reg[RT(i)]; },
    [](u32 i) // addu 0x21
    { reg[RD(i)] = reg[RS(i)] + reg[RT(i)]; },
    [](u32 i) // sub 0x22
    { reg[RD(i)] = reg[RS(i)] - reg[RT(i)]; },
    [](u32 i) // subu 0x23
    { reg[RD(i)] = reg[RS(i)] - reg[RT(i)]; },
    [](u32 i) // and_ 0x24
    { reg[RD(i)] = reg[RS(i)] & reg[RT(i)]; },
    [](u32 i) // or_ 0x25
    { reg[RD(i)] = reg[RS(i)] | reg[RT(i)]; },
    [](u32 i) // xor_ 0x26
    { reg[RD(i)] = reg[RS(i)] ^ reg[RT(i)]; },
    [](u32 i) // nor 0x27
    { reg[RD(i)] = !(reg[RS(i)] | reg[RT(i)]); },
    trap,     // 0x28
    trap,     // 0x29
    [](u32 i) // slt 0x2A
    { reg[RD(i)] = static_cast<i32>(reg[RS(i)]) < static_cast<i32>(reg[RT(i)]); },
    [](u32 i) // sltu 0x2B
    { reg[RD(i)] = reg[RT(i)] < reg[RT(i)]; },
    trap, // 0x2C
    trap, // 0x2D
    trap, // 0x2E
    trap, // 0x2F
    trap, // 0x30
    trap, // 0x31
    trap, // 0x32
    trap, // 0x33
    trap, // 0x34
    trap, // 0x35
    trap, // 0x36
    trap, // 0x37
    trap, // 0x38
    trap, // 0x39
    trap, // 0x3A
    trap, // 0x3B
    trap, // 0x3C
    trap, // 0x3D
    trap, // 0x3E
    trap, // 0x3F
};

inline const exec_handle sp2[64] = {
    [](u32 i) // madd 0x00
    {
        u64 res = hi * (1ULL << 32ULL) + lo;
        res += 1LL * static_cast<i32>(reg[RS(i)]) * static_cast<i64>(reg[RT(i)]);
        hi = res >> 32ULL;
        lo = res & 0xFFFFFFFFULL;
    },
    [](u32 i) // maddu 0x01
    {
        u64 res = hi * (1ULL << 32ULL) + lo;
        res += 1ULL * reg[RS(i)] * reg[RT(i)];
        hi = res >> 32ULL;
        lo = res & 0xFFFFFFFFULL;
    },
    [](u32 i) // mul 0x02
    { reg[RD(i)] = (1LL * reg[RS(i)] * reg[RT(i)]) & 0xFFFFFFFFULL; },
    trap,     // 0x03
    [](u32 i) // msub 0x04
    {
        u64 res = hi * (1ULL << 32ULL) + lo;
        res = 1LL * static_cast<i32>(reg[RS(i)]) * static_cast<i64>(reg[RT(i)]) - res;
        hi = res >> 32ULL;
        lo = res & 0xFFFFFFFFULL;
    },
    [](u32 i) // msubu 0x05
    {
        u64 res = hi * (1ULL << 32ULL) + lo;
        res = 1ULL * reg[RS(i)] * reg[RT(i)] - res;
        hi = res >> 32ULL;
        lo = res & 0xFFFFFFFFULL;
    },
    trap,     // 0x06
    trap,     // 0x07
    trap,     // 0x08
    trap,     // 0x09
    trap,     // 0x0A
    trap,     // 0x0B
    trap,     // 0x0C
    trap,     // 0x0D
    trap,     // 0x0E
    trap,     // 0x0F
    trap,     // 0x10
    trap,     // 0x11
    trap,     // 0x12
    trap,     // 0x13
    trap,     // 0x14
    trap,     // 0x15
    trap,     // 0x16
    trap,     // 0x17
    trap,     // 0x18
    trap,     // 0x19
    trap,     // 0x1A
    trap,     // 0x1B
    trap,     // 0x1C
    trap,     // 0x1D
    trap,     // 0x1E
    trap,     // 0x1F
    [](u32 i) // clz 0x20
    { reg[RD(i)] = std::countl_zero(reg[RS(i)]); },
    [](u32 i) // clo 0x21
    { reg[RD(i)] = std::countl_one(reg[RS(i)]); },
    trap, // 0x22
    trap, // 0x23
    trap, // 0x24
    trap, // 0x25
    trap, // 0x26
    trap, // 0x27
    trap, // 0x28
    trap, // 0x29
    trap, // 0x2A
    trap, // 0x2B
    trap, // 0x2C
    trap, // 0x2D
    trap, // 0x2E
    trap, // 0x2F
    trap, // 0x30
    trap, // 0x31
    trap, // 0x32
    trap, // 0x33
    trap, // 0x34
    trap, // 0x35
    trap, // 0x36
    trap, // 0x37
    trap, // 0x38
    trap, // 0x39
    trap, // 0x3A
    trap, // 0x3B
    trap, // 0x3C
    trap, // 0x3D
    trap, // 0x3E
    trap, // 0x3F
};

inline const exec_handle exec[64] = {
    [](u32 i) // spec1 0x00
    {
        u32 fn = FN(i);
        sp1[fn](i);
    },
    [](u32 i) { // bltz_bgez 0x01
        i32 offset = static_cast<i16>(IMM(i));
        u32 _pc = pc + 4;
        if ((!reg[RT(i)]) == (reg[RS(i)] < 0)) pc += (offset << 2);
        // else
        //     pc += 4;
        // execute_delayslot(_pc);
    },
    [](u32 i) // j 0x02
    {
        u32 tar = TAR(i);
        u32 _pc = pc + 4;
        pc = ((pc & 0xF0000000U) | (tar << 2)) - 4;
        // execute_delayslot(_pc);
    },
    [](u32 i) // jal 0x03
    {
        u32 tar = TAR(i);
        u32 _pc = pc + 4;
        ra = pc + 4; // 8;
        pc = ((pc & 0xF0000000U) | (tar << 2)) - 4;
        // execute_delayslot(_pc);
    },
    [](u32 i) // beq 0x04
    {
        u32 _pc = pc + 4;
        i32 offset = static_cast<i16>(IMM(i));
        if (reg[RS(i)] == reg[RT(i)]) pc += (offset << 2);
        // else
        //     pc += 4;
        // execute_delayslot(_pc);
    },
    [](u32 i) // bne 0x05
    {
        u32 _pc = pc + 4;
        i32 offset = static_cast<i16>(IMM(i));
        if (reg[RS(i)] != reg[RT(i)]) pc += (offset << 2);
        // else
        //     pc += 4;
        // execute_delayslot(_pc);
    },
    [](u32 i) // blez 0x06
    {
        u32 _pc = pc + 4;
        i32 offset = static_cast<i16>(IMM(i));
        if (static_cast<i32>(reg[RS(i)]) <= 0) pc += (offset << 2);
        // else
        //     pc += 4;
        // execute_delayslot(_pc);
    },
    [](u32 i) // bgtz 0x07
    {
        u32 _pc = pc + 4;
        i32 offset = static_cast<i16>(IMM(i));
        if (static_cast<i32>(reg[RS(i)]) > 0) pc += (offset << 2);
        // else
        //     pc += 4;
        // execute_delayslot(_pc);
    },
    [](u32 i) // addi 0x08
    {
        u32 num = static_cast<i16>(IMM(i));
        reg[RT(i)] = reg[RS(i)] + num;
    },
    [](u32 i) // addiu 0x09
    {
        u32 num = static_cast<i16>(IMM(i));
        reg[RT(i)] = reg[RS(i)] + num;
    },
    [](u32 i) // slti 0x0A
    {
        u32 num = static_cast<i16>(IMM(i));
        reg[RT(i)] = reg[RS(i)] < num;
    },
    [](u32 i) // sltiu 0x0B
    {
        u32 num = static_cast<i16>(IMM(i));
        reg[RT(i)] = static_cast<i32>(reg[RS(i)]) < static_cast<i32>(num);
    },
    [](u32 i) // andi 0x0C
    {
        u32 num = IMM(i);
        reg[RT(i)] = reg[RS(i)] & num;
    },
    [](u32 i) // ori 0x0D
    {
        u32 num = IMM(i);
        reg[RT(i)] = reg[RS(i)] | num;
    },
    [](u32 i) // xori 0x0E
    {
        u32 num = IMM(i);
        reg[RT(i)] = reg[RS(i)] ^ num;
    },
    [](u32 i) // lui 0x0F
    { reg[RT(i)] = IMM(i) << 16U; },
    trap,     // _control_cop0 0x10
    trap,     // _fp_cop1 0x11
    trap,     // _cop2 0x12
    trap,     // 0x13
    trap,     // _beql 0x14
    trap,     // _bnql 0x15
    trap,     // _blezl 0x16
    trap,     // _bgtzl 0x17
    trap,     // 0x18
    trap,     // 0x19
    trap,     // 0x1A
    trap,     // 0x1B
    [](u32 i) // spec2 0x1C
    { sp2[FN(i)](i); },
    trap,     // 0x1D
    trap,     // 0x1E
    trap,     // 0x1F
    [](u32 i) // lb 0x20
    { reg[RT(i)] = static_cast<u32>((i8)byte_at(reg[RS(i)] + static_cast<i16>(IMM(i)))); },
    [](u32 i) // lh 0x21
    { reg[RT(i)] = static_cast<u32>((i16)hw_at(reg[RS(i)] + static_cast<i16>(IMM(i)))); },
    // FIXME
    [](u32 i) // lwl 0x22
    {
        u32 addr = reg[RS(i)] + static_cast<i16>(IMM(i));
        u32 stub = addr & 0x03;
        u32 mem_off = 24, reg_mask = 0xFF000000;
        for (u32 i = stub; i != 0xFFFFFFFFu; --i)
        {
            reg[RT(i)] &= ~reg_mask;
            reg[RT(i)] |= static_cast<u32>(mem[addr - stub + i]) << mem_off;
            mem_off -= 8;
            reg_mask >>= 8;
        }
    },
    [](u32 i) // lw 0x23
    { reg[RT(i)] = w_at(reg[RS(i)] + static_cast<i16>(IMM(i))); },
    [](u32 i) // lbu 0x24
    { reg[RT(i)] = byte_at(reg[RS(i)] + static_cast<i16>(IMM(i))); },
    [](u32 i) // lhu 0x25
    { reg[RT(i)] = hw_at(reg[RS(i)] + static_cast<i16>(IMM(i))); },
    [](u32 i) // lwr 0x26
    {
        u32 addr = reg[RS(i)] + static_cast<i16>(IMM(i));
        u32 stub = addr & 0x03;
        u32 mem_off = 0, reg_mask = 0xFF;
        for (u32 i = stub; i != 4; ++i)
        {
            reg[RT(i)] &= ~reg_mask;
            reg[RT(i)] |= static_cast<u32>(mem[addr - stub + i]) << mem_off;
            mem_off += 8;
            reg_mask <<= 8;
        }
    },
    trap,     // 0x27
    [](u32 i) // sb 0x28
    { sbyte_to(reg[RS(i)] + static_cast<i16>(IMM(i)), reg[RT(i)] & 0xFFU); },
    [](u32 i) // sh 0x29
    { shw_to(reg[RS(i)] + static_cast<i16>(IMM(i)), reg[RT(i)] & 0xFFFFU); },
    [](u32 i) // swl 0x2A
    {
        u32 addr = reg[RS(i)] + static_cast<i16>(IMM(i));
        u32 stub = addr & 0x03;
        u32 mem_off = 24, reg_mask = 0xFF000000;
        for (u32 i = stub; i != 0xFFFFFFFFu; --i)
        {
            mem[addr - stub + i] = (reg[RT(i)] & reg_mask) >> mem_off;
            mem_off -= 8;
            reg_mask >>= 8;
        }
    },
    [](u32 i) // sw 0x2B
    { sw_to(reg[RS(i)] + static_cast<i16>(IMM(i)), reg[RT(i)]); },
    trap,     // 0x2C,
    trap,     // 0x2D,
    [](u32 i) // swr 0x2E
    {
        u32 addr = reg[RS(i)] + static_cast<i16>(IMM(i));
        u32 stub = addr & 0x03;
        u32 mem_off = 0, reg_mask = 0xFF;
        for (u32 i = stub; i != 4; ++i)
        {
            mem[addr - stub + i] = (reg[RT(i)] & reg_mask) >> mem_off;
            mem_off += 8;
            reg_mask <<= 8;
        }
    },
    trap, // 0x2E
};

inline void execute_delayslot(u32 addr)
{
    u32 i = w_at(addr);
    exec[OP(i)](i);
}

inline void execute(u32 instr)
{
    XLDZ_DEBUGF("Executing instruction %08x at .text+%d\n", instr, pc - TEXT_BEGIN);
    for (int i = 4; i < 28; i += 4)
    {
        XLDZ_DEBUGF("%08x at STACK-%d\n", w_at(STACK_BEGIN - i), i);
    }
    exec[OP(instr)](instr);
}

} // namespace xldz

#endif
