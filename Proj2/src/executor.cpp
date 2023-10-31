#include <executor.h>
#include <snapshot.h>

#include <cpu.hpp>
#include <memory.hpp>
#include <register.hpp>
namespace xldz
{
void execute()
{
    xldz::fp = xldz::STACK_BEGIN;
    xldz::sp = xldz::STACK_BEGIN;
    xldz::pc = xldz::TEXT_BEGIN;
    xldz::gp = xldz::DATA_BEGIN + 0x8000;
    u32 cnt=0;
    while (1)
    {
        snapshot(cnt++);
        execute(w_at(pc));
        pc += 4;
    }
}
} // namespace xldz
