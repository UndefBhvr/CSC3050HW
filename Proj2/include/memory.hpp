#ifndef XLDZ_MEMORY_HPP
#define XLDZ_MEMORY_HPP

#include <prelude.hpp>

namespace xldz
{

constexpr u32 MEM_SIZE = 0x600000;
constexpr u32 TEXT_BEGIN = 0x400000;
constexpr u32 DATA_BEGIN = 0x500000;
constexpr u32 STACK_BEGIN = 0xA00000;
inline u8 real_mem[MEM_SIZE];

inline u8 *mem = real_mem - TEXT_BEGIN;

inline u8 *stack = mem + STACK_BEGIN;

inline u8 *data_pos = mem + DATA_BEGIN;

inline u8 byte_at(u32 addr)
{
    return mem[addr];
}

inline void sbyte_to(u32 addr, u8 val)
{
    mem[addr] = val;
}

inline u16 hw_at(u32 addr)
{
    return mem[addr] + ((u16)mem[addr + 1] << 8U);
}

inline void shw_to(u32 addr, u16 val)
{
    mem[addr] = val & 0xFFU;
    mem[addr + 1] = (val & 0xFF00U) >> 8U;
}

inline u32 w_at(u32 addr)
{
    return mem[addr] + ((u32)mem[addr + 1] << 8U) + ((u32)mem[addr + 2] << 16U) +
           ((u32)mem[addr + 3] << 24U);
}

inline void sw_to(u32 addr, u32 val)
{
    mem[addr] = val & 0xFFU;
    mem[addr + 1] = (val & 0xFF00U) >> 8U;
    mem[addr + 2] = (val & 0xFF0000U) >> 16U;
    mem[addr + 3] = (val & 0xFF000000U) >> 24U;
}

inline char *string_at(u32 addr)
{
    return reinterpret_cast<char *>(mem + addr);
}

} // namespace xldz

#endif
