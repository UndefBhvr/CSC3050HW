#include <cstdio>
#include <fstream>

#include <executor.h>
#include <loader.h>
#include <snapshot.h>

#include <memory.hpp>
#include <prelude.hpp>
#include <register.hpp>



int main(int argc, char **argv)
{
    std::ifstream asm_file(argv[1]);
    std::ifstream bin_file(argv[2]);
    xldz::init_checkpoints(argv[3]);
    freopen(argv[4],"r",stdin);
    freopen(argv[5],"w",stdout);
    xldz::load_data(asm_file);
    xldz::binary_u32_stream instr_stream(bin_file);
    xldz::load_instr(instr_stream);
    XLDZ_DEBUGF("data_pos at 0x%08x\n", xldz::data_pos - xldz::mem);
    for (unsigned char *p = xldz::mem + xldz::DATA_BEGIN; p != xldz::data_pos; ++p)
    {
        XLDZ_DEBUGF("%02x", *p);
        if ((p - xldz::mem) % 4 == 3) XLDZ_DEBUGF("\n");
    }
    for (unsigned i = 0; i < 4 * 44; i += 4)
    {
        XLDZ_DEBUGF("%08x at .text+%d\n", xldz::w_at(xldz::TEXT_BEGIN + i), i);
    }
    xldz::execute();
}
