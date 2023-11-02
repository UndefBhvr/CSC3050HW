#ifndef XLDZ_PRELUDE_HPP
#define XLDZ_PRELUDE_HPP

#include <cstdint>

namespace xldz
{

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

constexpr u32 OP_OFF = 26, OP_MASK = 0xFC000000;
constexpr u32 RS_OFF = 21, RS_MASK = 0x03E00000;
constexpr u32 RT_OFF = 16, RT_MASK = 0x001F0000;
constexpr u32 RD_OFF = 11, RD_MASK = 0x0000F800;
constexpr u32 SA_OFF = 6, SA_MASK = 0x000007C0;
constexpr u32 FN_OFF = 0, FN_MASK = 0x0000003F;
constexpr u32 IMM_OFF = 0, IMM_MASK = 0x0000FFFF;
constexpr u32 TAR_OFF = 0, TAR_MASK = 0x03FFFFFF;

#define GEN_GETTER(name)                        \
    inline u32 name(u32 i)                      \
    {                                           \
        return (i & name##_MASK) >> name##_OFF; \
    }

GEN_GETTER(OP);
GEN_GETTER(RS);
GEN_GETTER(RT);
GEN_GETTER(RD);
GEN_GETTER(SA);
GEN_GETTER(FN);
GEN_GETTER(IMM);
GEN_GETTER(TAR);

#undef GEN_GETTER

} // namespace xldz

#define NDEBUG

#ifndef NDEBUG
#    define XLDZ_DEBUGF(fmt, ...) fprintf(stderr, fmt __VA_OPT__(, ) __VA_ARGS__)
#else
#    define XLDZ_DEBUGF(fmt, ...)
#endif

#endif
