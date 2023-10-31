#include <cpu.hpp>
#include <executor.h>
#include <memory.hpp>
#include <register.hpp>
namespace xldz
{
void execute()
{
    xldz::fp = xldz::STACK_BEGIN;
    xldz::sp = xldz::STACK_BEGIN;
    xldz::pc = xldz::TEXT_BEGIN;
    while (1)
    {
        execute(w_at(pc));
        pc += 4;
    }
}
} // namespace xldz
