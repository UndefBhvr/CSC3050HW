#ifndef UNDEFBHVR_MIPS_HPP
#define UNDEFBHVR_MIPS_HPP

#include <cctype>
#include <cstddef>
#include <exception>
#include <instr_parse/ISA_common.hpp>
#include <lex/lexer.hpp>
#include <istream>
#include <map>
#include <string>





namespace MIPS
{

enum Instruction_type
{
    R,
    I,
    J
};

// enum class Argument_type
// {
//     RT,
//     RS,
//     RD,
//     SA,
//     IMM,
//     OFF_IMM,
//     B_LAB,
//     J_LAB
// };

struct Data_field
{
    unsigned short rs : 5 = 0, rt : 5 = 0, rd : 5 = 0, sa : 5 = 0;
    unsigned imm = 0;
    std::string lab = "";
};

inline Data_field operator|(const Data_field &lhs, const Data_field &rhs)
{
    auto [a, b, c, d, e, f] = lhs;
    auto [x, y, z, u, v, w] = rhs;
    return Data_field(a | x, b | y, c | z, d | u, e | v, f.empty() ? w : f);
}

inline Data_field &operator|=(Data_field &lhs, const Data_field &rhs)
{
    return lhs = lhs | rhs;
}

inline unsigned short parse_reg(const std::string &name)
{
    if (name.size() < 2)
    {
        if(isdigit(name[0]))return name[0]-'0';
        return 0xffff;
    }
    switch (name[0])
    {
    case 'z':
        if (name == "zero") return 0;
        return 0xffff;
    case 'a':
        if (name == "at") return 1;
        if (name.size() == 2 && name[1] >= '0' && name[1] <= '3') return 4 + name[1] - '0';
        return 0xffff;
    case 'v':
        if (name.size() == 2 && name[1] >= '0' && name[1] <= '1') return 2 + name[1] - '0';
        return 0xffff;
    case 't':
        if (name.size() == 2 && name[1] >= '0' && name[1] <= '7') return 8 + name[1] - '0';
        if (name.size() == 2 && name[1] >= '8' && name[1] <= '9') return 24 + name[1] - '8';
        return 0xffff;
    case 's':
        if (name == "sp") return 29;
        if (name.size() == 2 && name[1] >= '0' && name[1] <= '7') return 16 + name[1] - '0';
        return 0xffff;
    case 'k':
        if (name.size() == 2 && name[1] >= '0' && name[1] <= '1') return 26 + name[1] - '0';
        return 0xffff;
    case 'g':
        if (name == "gp") return 28;
        return 0xffff;
    case 'f':
        if (name == "fp") return 30;
        return 0xffff;
    case 'r':
        if (name == "ra") return 31;
        return 0xffff;
    default:
        if(name.size()==2&&isdigit(name[0])&&isdigit(name[1]))
        {
            unsigned short res=(name[0]-'0')*10+name[1]-'0';
            if(res<32)return res;
        }
        return 0xffff;
    }
}

inline int parse_int(const std::string &num)
{
    if (num == "0") return 0;
    int res = 0;
    if (num.starts_with("0x"))
    {
        for (std::size_t i = 2; i != num.size(); ++i)
        {
            res *= 16;
            if (isdigit(num[i]))
                res += num[i] - '0';
            else if (isupper(num[i]))
                res += num[i] - 'A';
            else if (islower(num[i]))
                res += num[i] - 'a';
        }
        return res;
    }
    if (num[0] == 0)
    {
        for (std::size_t i = 1; i != num.size(); ++i)
        {
            res *= 8;
            res += num[i] - '0';
        }
        return res;
    }
    bool sym = 0;
    if (num[0] == '-') sym = 1;
    for (std::size_t i = isdigit(num[0]) ? 0 : 1; i != num.size(); ++i)
    {
        res *= 10;
        res += num[i] - '0';
    }
    return sym ? -res : res;
}


struct rs
{
    static Data_field parse(lexer::token_stream::iterator &it)
    {
        if (it->type != lexer::token::REG) std::terminate();
        Data_field res;
        res.rs = parse_reg(it->content);
        ++it;
        return res;
    }
};

struct rt
{
    static Data_field parse(lexer::token_stream::iterator &it)
    {
        if (it->type != lexer::token::REG) std::terminate();
        Data_field res;
        res.rt = parse_reg(it->content);
        ++it;
        return res;
    }
};

template<unsigned rt_v>
struct irt
{
    static Data_field parse(lexer::token_stream::iterator &)
    {
        Data_field res;
        res.imm = rt_v;
        return res;
    }
};

struct rd
{
    static Data_field parse(lexer::token_stream::iterator &it)
    {
        if (it->type != lexer::token::REG) std::terminate();
        Data_field res;
        res.rd = parse_reg(it->content);
        ++it;
        return res;
    }
};

struct sa
{
    static Data_field parse(lexer::token_stream::iterator &it)
    {
        if (it->type != lexer::token::REG) std::terminate();
        Data_field res;
        res.sa = parse_reg(it->content);
        ++it;
        return res;
    }
};

template<typename Base>
struct off
{
    static Data_field parse(lexer::token_stream::iterator &it)
    {
        if (it->type != lexer::token::NUM) std::terminate();
        Data_field res;
        res.imm = parse_int(it->content);
        ++it;
        if (it->type != lexer::token::SYM || it->content != "(") std::terminate();
        ++it;
        res |= Base::parse(it);
        if (it->type != lexer::token::SYM || it->content != ")") std::terminate();
        ++it;
        return res;
    }
};

struct imm
{
    static Data_field parse(lexer::token_stream::iterator &it)
    {
        if (it->type != lexer::token::NUM) std::terminate();
        Data_field res;
        res.imm = parse_int(it->content);
        ++it;
        return res;
    }
};

struct blab
{
    static Data_field parse(lexer::token_stream::iterator &it)
    {
        if (it->type != lexer::token::ID) std::terminate();
        Data_field res;
        res.lab = it->content;
        ++it;
        return res;
    }
};

struct jlab
{
    static Data_field parse(lexer::token_stream::iterator &it)
    {
        if (it->type != lexer::token::ID) std::terminate();
        Data_field res;
        res.lab = it->content;
        ++it;
        return res;
    }
};

template<typename... Args>
using arg_t = common::args<Data_field, Args...>;

template<typename... Args>
inline auto arg = arg_t<Args...>();

template<typename... Args>
using none_t = common::none<Data_field, Args...>;

template<typename... Args>
inline auto none = none_t<Args...>();

// inline auto none=

struct instr
{
    Instruction_type type : 2;
    unsigned op : 6, fn : 6;
    common::arg_base<Data_field> *parser;
};

inline const std::map<std::string, instr> mapper{
    {"add", {R, 0x00, 0x20, &arg<rd, rs, rt>}},
    {"addu", {R, 0x00, 0x21, &arg<rd, rs, rt>}},
    {"and", {R, 0x00, 0x24, &arg<rd, rs, rt>}},
    {"div", {R, 0x00, 0x1A, &arg<rs, rt>}},
    {"divu", {R, 0x00, 0x1B, &arg<rs, rt>}},
    {"jalr", {R, 0x00, 0x09, &arg<rd, rs>}},
    {"jr", {R, 0x00, 0x08, &arg<rs>}},
    {"mfhi", {R, 0x00, 0x10, &arg<rd>}},
    {"mflo", {R, 0x00, 0x12, &arg<rd>}},
    {"mthi", {R, 0x00, 0x11, &arg<rs>}},
    {"mtlo", {R, 0x00, 0x13, &arg<rs>}},
    {"mult", {R, 0x00, 0x18, &arg<rs, rt>}},
    {"multu", {R, 0x00, 0x19, &arg<rs, rt>}},
    {"nor", {R, 0x00, 0x27, &arg<rd, rs, rt>}},
    {"or", {R, 0x00, 0x25, &arg<rd, rs, rt>}},
    {"sll", {R, 0x00, 0x00, &arg<rd, rt, sa>}},
    {"sllv", {R, 0x00, 0x04, &arg<rd, rt, rs>}},
    {"slt", {R, 0x00, 0x2A, &arg<rd, rs, rt>}},
    {"sltu", {R, 0x00, 0x2B, &arg<rd, rs, rt>}},
    {"sra", {R, 0x00, 0x03, &arg<rd, rt, sa>}},
    {"srav", {R, 0x00, 0x07, &arg<rd, rt, rs>}},
    {"srl", {R, 0x00, 0x02, &arg<rd, rt, sa>}},
    {"srlv", {R, 0x00, 0x06, &arg<rd, rt, rs>}},
    {"sub", {R, 0x00, 0x22, &arg<rd, rs, rt>}},
    {"subu", {R, 0x00, 0x23, &arg<rd, rs, rt>}},
    {"syscall", {R, 0x00, 0x0C, &none<>}},
    {"xor", {R, 0x00, 0x26, &arg<rd, rs, rt>}},
    {"addi", {I, 0x08, 0x00, &arg<rt, rs, imm>}},
    {"addiu", {I, 0x09, 0x00, &arg<rt, rs, imm>}},
    {"andi", {I, 0x0C, 0x00, &arg<rt, rs, imm>}},
    {"beq", {I, 0x04, 0x00, &arg<rs, rt, blab>}},
    {"bgez", {I, 0x01, 0x00, &arg<rs, blab, none_t<irt<0x01>>>}},
    {"bgtz", {I, 0x07, 0x00, &arg<rs, blab, none_t<irt<0x00>>>}},
    {"blez", {I, 0x06, 0x00, &arg<rs, blab, none_t<irt<0x00>>>}},
    {"bltz", {I, 0x01, 0x00, &arg<rs, blab, none_t<irt<0x00>>>}},
    {"bne", {I, 0x05, 0x00, &arg<rs, rt, blab>}},
    {"lb", {I, 0x20, 0x00, &arg<rt, off<rs>>}},
    {"lbu", {I, 0x24, 0x00, &arg<rt, off<rs>>}},
    {"lh", {I, 0x21, 0x00, &arg<rt, off<rs>>}},
    {"lhu", {I, 0x25, 0x00, &arg<rt, off<rs>>}},
    {"lui", {I, 0x0F, 0x00, &arg<rt, imm>}},
    {"lw", {I, 0x23, 0x00, &arg<rt, off<rs>>}},
    {"lwc1", {I, 0x31, 0x00, &arg<rt, off<rs>>}},
    {"ori", {I, 0x0D, 0x00, &arg<rt, rs, imm>}},
    {"sb", {I, 0x28, 0x00, &arg<rt, off<rs>>}},
    {"slti", {I, 0x0A, 0x00, &arg<rt, rs, imm>}},
    {"sltiu", {I, 0x0B, 0x00, &arg<rt, rs, imm>}},
    {"sh", {I, 0x29, 0x00, &arg<rt, off<rs>>}},
    {"sw", {I, 0x2B, 0x00, &arg<rt, off<rs>>}},
    {"swc1", {I, 0x39, 0x00, &arg<rt, off<rs>>}},
    {"xori", {I, 0x0E, 0x00, &arg<rt, rs, imm>}},
    {"lwl", {I, 0x22, 0x00, &arg<rt, off<rs>>}},
    {"lwr", {I, 0x26, 0x00, &arg<rt, off<rs>>}},
    {"swl", {I, 0x2A, 0x00, &arg<rt, off<rs>>}},
    {"swr", {I, 0x2E, 0x00, &arg<rt, off<rs>>}},
    {"j", {J, 0x02, 0x00, &arg<jlab>}},
    {"jal", {J, 0x03, 0x00, &arg<jlab>}}


};

} // namespace MIPS

#endif