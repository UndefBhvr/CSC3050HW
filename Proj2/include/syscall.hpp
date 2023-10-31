#ifndef XLDZ_SYSCALL_HPP
#define XLDZ_SYSCALL_HPP

#include <memory.hpp>
#include <register.hpp>

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

namespace xldz
{

    using syscall_handler = void (*)();

    inline syscall_handler sysh[32];

    inline void syscall()
    {
        sysh[v0]();
    }

    inline void reg_syscall_handle(u32 code, syscall_handler hndl)
    {
        sysh[code] = hndl;
    }

    namespace
    {
        inline u32 _init = []() {
            reg_syscall_handle(1, []() { printf("%d", a0); });
            reg_syscall_handle(4, []() {
                XLDZ_DEBUGF("Printing string at %08x\n", a0);
                printf("%s", string_at(a0));
            });
            reg_syscall_handle(5, []() { scanf("%d", &v0); });
            reg_syscall_handle(8,
                               []() {
                                   char c = getchar();
                                   while (c == '\n')
                                       c = getchar();
                                   for (u32 i = 0; i < a1 - 1; ++i)
                                   {
                                       mem[a0 + i] = c;
                                       if (mem[a0 + i] == '\n')
                                           return;
                                       c = getchar();
                                   }
                                   mem[a0 + a1 - 1] = c;
                               });
            reg_syscall_handle(9, []() { v0 = (brk_pos += a0) - mem; });
            reg_syscall_handle(10, []() { exit(0); });
            reg_syscall_handle(11, []() { putchar(a0); });
            reg_syscall_handle(12, []() { v0 = getchar(); });
            reg_syscall_handle(13, []() { a0 = open(string_at(a0), a1, a2); });
            reg_syscall_handle(14, []() { a0 = read(a0, string_at(a1), a2); });
            reg_syscall_handle(15, []() { a0 = write(a0, string_at(a1), a2); });
            reg_syscall_handle(16, []() { close(a0); });
            reg_syscall_handle(17, []() { exit(a0); });
            return 0;
        }();
    }

} // namespace xldz

#endif
