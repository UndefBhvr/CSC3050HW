# XLDZ: An MIPS-Like Simulator

### Big picture thoughts & ideas

A program is loaded into the memory at proper position and then executed.

This simulator simulates the loader, the memory, and the cpu including register files.

### High-level implementation ideas

The data are loaded from asm files, which can be parsed by the parser of project1.

The instructions are loaded from binary files so there's no need to parse it.

MIPS instructions can be roughly divided into several types:

- I-type
- R-type (spec1 and spec2)
- J-type
- coprocessor instructions (not implemented yet)

The main type of an instruction can be recognized from the op-code.

The subtype of an instruction can be recognized from other part like fn-code for spec1 and spec2.

Therefore, we can use simple structures like arrays to hold a jump table. Each instruction will jump to the corresponding handler function.

GPRs can be stored in a uint32_t[32].

Memory is an big array with length of 0x600000 bytes.

Since the address space below .text section is preserved, the actual memory begins at .text.

To handle this, there are two types of address: physical address(paddr) and virtual address(vaddr). The mapping function is vaddr=paddr+0x400000.

### Implementation details

Since the using of fixed-width integers is very common in this project, several type aliases are defined in `prelude.hpp`.

```cpp
//prelude.hpp
namespace xldz
{  
    using i8=int8_t;
    using i16=int16_t;
    using i32=int32_t;
    using i64=int64_t;
    using u8=uint8_t;
    using u16=uint16_t;
    using u32=uint32_t;
    using u64=uint64_t;
}
```

To simplify the declaration of similar parts, some macros are used to generate the declaration. For example, register aliases and instruction-part getters(RS(u32),IMM(u32),etc) are generated by macros.

For debugging, a macro `XLDZ_DEBUGF(fmt,...)` is defined. When `NDEBUG` is not defined, it will be expanded to `fprintf(stderr,fmt,...)`.

Like project1, `xldz::token_stream` consumes the assembly source and produces a range of tokens. After ignoring the labels in .data section, it is easy to load the data into .data section by using `xldz::load_data`.

`xldz::load_instr` consumes a range of uint32_t and regards the integers as instructions. They are loaded into .text section.

`xldz::execute()` initialize the registers and then run the program from the entry point (`.text + 0`). `$sp` and `$fp` are initialized as `0xA00000` (begin of the stack). `$gp` is initialized as `0x508000 (.data+32kB)`.

`xldz::execute(u32)` selected an `exec_handle (aka void(*)(u32))` from an array, using op-code as the index.

For instructions of spec1 and spec2, there are other two `exec_handle` arrays for selecting proper handlers.

All the handlers are implemented as lambda expressions written in `cpu.hpp`.

For syscalls, most of them have corresponding APIs in standard library or posix APIs. I mainly developed this project on Windows, but I can use MinGW for those posix functions.

Also, `syscall.hpp` gives an API `reg_syscall_handle(u32,void(*)())` that allow users to register handle functions dynamically.

Snapshot part is almost the same as the template on bb. The main modification is printing the registers using a loop instead of list all the register aliases.

### Future

There will probably be FPU support, Tomasulo algorithm with ROB, etc. in the future.

### About the name

X <=> M

L <=> I

D <=> P

Z <=> S

Each pair is similar.